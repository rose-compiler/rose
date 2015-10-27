#include "sage3basic.h"
#include "AST_FILE_IO.h"
#include "CloneDetectionLib.h"
#include "Combinatorics.h"
#include "BinaryLoaderElf.h"
#include "Partitioner.h"
#include "rose.h"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

using namespace rose::BinaryAnalysis;

namespace CloneDetection {

const rose_addr_t GOTPLT_VALUE = 0x09110911; // Address of all dynamic functions that are not loaded



/*******************************************************************************************************************************
 *                                      Exceptions
 *******************************************************************************************************************************/

std::ostream &
operator<<(std::ostream &o, const Exception &e)
{
    o <<e.mesg;
    return o;
}

/*******************************************************************************************************************************
 *                                      Tracer
 *******************************************************************************************************************************/

void
Tracer::current_test(int func_id, int igroup_id, unsigned event_mask, size_t pos)
{
    this->func_id = func_id;
    this->igroup_id = igroup_id;
    this->event_mask = event_mask;
    this->pos = pos;
}

void
Tracer::emit(rose_addr_t addr, TracerEvent event, uint64_t value, int minor)
{
    assert(func_id>=0 && igroup_id>=0);
    if (0!=(event & event_mask))
        insert(TracerRow(func_id, igroup_id, pos++, addr, event, minor, value));
}

/*******************************************************************************************************************************
 *                                      Progress
 *******************************************************************************************************************************/

void
Progress::init()
{
    is_terminal = isatty(2);
    update(true);
    cur = 0;
}

std::string
Progress::line() const
{
    size_t n = std::min(cur, total);
    int nchars = total>0 ? round((double)n/total * WIDTH) : 0;
    std::string bar(nchars, '=');
    bar += std::string(WIDTH-nchars, ' ');
    if (!mesg.empty()) {
        assert(WIDTH>9);
        size_t mesg_sz = std::min((size_t)WIDTH-9, mesg.size());
        std::string s = mesg_sz>=mesg.size() ? mesg : mesg.substr(0, mesg_sz)+"...";
        bar.replace(3, s.size(), s);
    }
    std::ostringstream ss;
    ss <<" " <<std::setw(3) <<(total>0?(int)round(100.0*n/total):0) <<"% " <<cur <<"/" <<total <<" |" <<bar <<"|";
    return ss.str();
}

void
Progress::update(bool update_now)
{
    if ((force || is_terminal) && total>0) {
        if ((size_t)(-1)==total) {
            if (had_output)
                fputc('\n', stderr);
        } else {
            if (!update_now)
                update_now = !had_output || time(NULL) - last_report > RPT_INTERVAL;
            if (update_now) {
                std::string s = line();
                fputs(s.c_str(), stderr);
                fputc(is_terminal?'\r':'\n', stderr);
                fflush(stderr);
                last_report = time(NULL);
                had_output = true;
            }
        }
    }
}

void
Progress::increment(bool update_now)
{
    update(update_now || cur==total);
    ++cur;
}

void
Progress::clear()
{
    if (had_output) {
        if (is_terminal) {
            std::string s = line();
            fprintf(stderr, "\r%*s\r", (int)s.size(), "");
            fflush(stderr);
        }
        had_output = false;
    }
}

void
Progress::reset(size_t current, size_t total)
{
    cur = current;
    if ((size_t)(-1)!=total)
        this->total = total;
    update();
}

void
Progress::message(const std::string &s, bool update_now)
{
    mesg = s;
    update(update_now);
}

/*******************************************************************************************************************************
 *                                      Files table
 *******************************************************************************************************************************/

void
FilesTable::load(const SqlDatabase::TransactionPtr &tx)
{
    clear();
    SqlDatabase::StatementPtr stmt = tx->statement("select id, name, digest, ast from semantic_files");
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        int id = row.get<int>(0);
        std::string name = row.get<std::string>(1);
        std::string digest = row.get<std::string>(2);
        std::string ast_digest = row.get<std::string>(3);
        rows[id] = Row(id, name, digest, ast_digest, true);
        name_idx[name] = id;
        next_id = std::max(next_id, id+1);
    }
}

void
FilesTable::save(const SqlDatabase::TransactionPtr &tx)
{
    SqlDatabase::StatementPtr stmt = tx->statement("insert into semantic_files"
                                                   // 0  1     2       3
                                                   "(id, name, digest, ast) values (?, ?, ?, ?)");
    for (Rows::iterator ri=rows.begin(); ri!=rows.end(); ++ri) {
        assert(ri->second.id==ri->first);
        if (!ri->second.in_db) {
            stmt->bind(0, ri->first);
            stmt->bind(1, ri->second.name);
            stmt->bind(2, ri->second.digest);
            stmt->bind(3, ri->second.ast_digest);
            stmt->execute();
            ri->second.in_db = true;
        }
    }
#ifndef NDEBUG
    int ndups = tx->statement("select count(*) from semantic_files f1"
                              " join semantic_files f2 on f1.id<>f2.id and f1.name=f2.name")->execute_int();
    assert(0==ndups);
#endif
}

bool
FilesTable::exists(const std::string &name) const
{
    return name_idx.find(name)!=name_idx.end();
}

int
FilesTable::insert(const std::string &name)
{
    NameIdx::iterator found = name_idx.find(name);
    if (found!=name_idx.end())
        return found->second;
    int id = next_id++;
    rows[id] = Row(id, name, "", "", false);
    name_idx[name] = id;
    return id;
}

std::string
FilesTable::save_ast(const SqlDatabase::TransactionPtr &tx, int64_t cmd_id, int file_id, SgProject *ast)
{
    Rows::iterator found = rows.find(file_id);
    assert(found!=rows.end());
    if (!ast) {
        found->second.ast_digest.clear();
    } else {
        found->second.ast_digest = CloneDetection::save_ast(tx, cmd_id);
    }
    if (found->second.in_db) {
        tx->statement("update semantic_files set ast = ? where id = ?")
            ->bind(0, found->second.ast_digest)
            ->bind(1, file_id)
            ->execute();
    }
    return found->second.ast_digest;
}

SgProject *
FilesTable::load_ast(const SqlDatabase::TransactionPtr &tx, int file_id)
{
    Rows::iterator found = rows.find(file_id);
    assert(found!=rows.end());
    if (found->second.ast_digest.empty())
        return NULL;
    return CloneDetection::load_ast(tx, found->second.ast_digest);
}

std::string
FilesTable::add_content(const SqlDatabase::TransactionPtr &tx, int64_t cmd_id, int file_id)
{
    Rows::iterator found = rows.find(file_id);
    assert(found!=rows.end());
    found->second.digest = save_binary_data(tx, cmd_id, found->second.name);
    if (found->second.in_db) {
        tx->statement("update semantic_files set digest = ? where id = ?")
            ->bind(0, found->second.digest)
            ->bind(1, file_id)
            ->execute();
    }
    return found->second.digest;
}

int
FilesTable::id(const std::string &name) const
{
    NameIdx::const_iterator found = name_idx.find(name);
    assert(found!=name_idx.end());
    return found->second;
}

std::string
FilesTable::name(int id) const
{
    Rows::const_iterator found = rows.find(id);
    assert(found!=rows.end());
    return found->second.name;
}

void
FilesTable::print(std::ostream &o) const
{
    for (NameIdx::const_iterator ni=name_idx.begin(); ni!=name_idx.end(); ++ni)
        o <<std::setw(4) <<ni->second <<" " <<ni->first <<"\n";
}

/*******************************************************************************************************************************
 *                                      Input groups
 *******************************************************************************************************************************/

void
InputQueue::load(int pos, uint64_t val)
{
    if (-1==pos) {
        pad_value_ = val;
        infinite_ = true;
    } else if (-2==pos) {
        InputQueueName to = (InputQueueName)val;
        assert(to>=0 && to<IQ_NQUEUES);
        redirect(to);
    } else {
        assert((size_t)pos==values_.size());
        values_.push_back(val);
    }
}

uint64_t
InputQueue::get(size_t idx) const
{
    if (idx<values_.size())
        return values_[idx];
    if (infinite_)
        return pad_value_;
    throw FaultException(AnalysisFault::INPUT_LIMIT);
}

std::vector<uint64_t> &
InputQueue::extend(size_t n)
{
    if (n>values_.size()) {
        if (!infinite_)
            throw FaultException(AnalysisFault::INPUT_LIMIT);
        values_.resize(n, pad_value_);
    }
    return values_;
}

bool
InputGroup::load(const SqlDatabase::TransactionPtr &tx, int igroup_id)
{
    bool retval = false;
    clear();
    SqlDatabase::StatementPtr stmt = tx->statement("select queue_id, pos, val"
                                                   " from semantic_inputvalues"
                                                   " where igroup_id = ?"
                                                   " order by queue_id, pos")->bind(0, igroup_id);
    collection_id = igroup_id;
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        if (-1==row.get<int>(0)) {
            collection_id = row.get<int>(2);
        } else {
            InputQueueName queue_id = (InputQueueName)row.get<int>(0);
            assert(queue_id>=0 && queue_id<IQ_NQUEUES);
            InputQueue &q = queue(queue_id);
            int pos = row.get<int>(1);
            uint64_t val = row.get<uint64_t>(2);
            q.load(pos, val);
            retval=true;
        }
    }
    return retval;
}

void
InputGroup::save(const SqlDatabase::TransactionPtr &tx, int igroup_id, int64_t cmd_id)
{
    SqlDatabase::StatementPtr stmt = tx->statement("insert into semantic_inputvalues"
                                                   // 0          1         2    3    4
                                                   " (igroup_id, queue_id, pos, val, cmd)"
                                                   " values (?, ?, ?, ?, ?)");
    stmt->bind(0, igroup_id);
    stmt->bind(4, cmd_id);
    for (size_t qn=0; qn<IQ_NQUEUES; ++qn) {
        InputQueue &q = queue((InputQueueName)qn);
        size_t nvals = q.size();
        stmt->bind(1, qn);
        for (size_t i=0; i<nvals; ++i)
            stmt->bind(2, i)->bind(3, q.get(i))->execute();
        if (q.is_infinite())
            stmt->bind(2, -1)->bind(3, q.get(nvals))->execute();
        if (q.redirect()!=IQ_NONE && q.redirect()!=(InputQueueName)qn)
            stmt->bind(2, -2)->bind(3, q.redirect())->execute();
    }
    if (collection_id>=0 && collection_id!=igroup_id)
        stmt->bind(1, -1)->bind(2, -1)->bind(3, collection_id)->execute();
}

size_t
InputGroup::nconsumed() const
{
    size_t retval = 0;
    for (Queues::const_iterator qi=queues_.begin(); qi!=queues_.end(); ++qi)
        retval += qi->nconsumed();
    return retval;
}

void
InputGroup::reset()
{
    for (Queues::iterator qi=queues_.begin(); qi!=queues_.end(); ++qi)
        qi->reset();
    nconsumed_virtual_ = std::vector<size_t>(IQ_NQUEUES, 0);
}

void
InputGroup::clear()
{
    for (Queues::iterator qi=queues_.begin(); qi!=queues_.end(); ++qi)
        qi->clear();
    collection_id = -1;
    nconsumed_virtual_ = std::vector<size_t>(IQ_NQUEUES, 0);
}

/*******************************************************************************************************************************
 *                                      Output groups
 *******************************************************************************************************************************/

bool
OutputGroup::operator<(const OutputGroup &other) const
{
    typedef std::pair<std::vector<int>::const_iterator, std::vector<int>::const_iterator> ii_pair;

    // scalar comparisons */
    if (fault != other.fault)
        return fault < other.fault;
    if (ninsns != other.ninsns)
        return ninsns < other.ninsns;
    if (has_retval != other.has_retval)
        return !has_retval;
    if (has_retval && retval!=other.retval)
        return retval < other.retval;

    // Values
    if (values < other.values)
        return true;

    // Function calls
    if (callee_ids.size() != other.callee_ids.size())
        return callee_ids.size() < other.callee_ids.size();
    ii_pair ii = std::mismatch(callee_ids.begin(), callee_ids.end(), other.callee_ids.begin());
    if (ii.first!=callee_ids.end())
        return *(ii.first) < *(ii.second);

    // System calls
    if (syscalls.size() != other.syscalls.size())
        return syscalls.size() < other.syscalls.size();
    ii = std::mismatch(syscalls.begin(), syscalls.end(), other.syscalls.begin());
    if (ii.first!=syscalls.end())
        return *(ii.first) < *(ii.second);

    return false;
}

bool
OutputGroup::operator==(const OutputGroup &other) const
{
    return (values==other.values &&
            callee_ids.size()==other.callee_ids.size() &&
            std::equal(callee_ids.begin(), callee_ids.end(), other.callee_ids.begin()) &&
            syscalls.size()==other.syscalls.size() &&
            std::equal(syscalls.begin(), syscalls.end(), other.syscalls.begin()) &&
            fault == other.fault &&
            ninsns == other.ninsns &&
            has_retval == other.has_retval &&
            (!has_retval || retval==other.retval));
}

void
OutputGroup::clear()
{
    values.clear();
    callee_ids.clear();
    syscalls.clear();
    fault = AnalysisFault::NONE;
    ninsns = 0;
    retval = 0;
    has_retval = false;
}

void
OutputGroup::print(std::ostream &o, const std::string &title, const std::string &prefix) const
{
    if (!title.empty())
        o <<title <<"\n";
    std::vector<value_type> valvec = values.get_vector();
    o <<prefix <<"values:";
    static const size_t width=100;
    size_t col = prefix.size()+7;
    for (size_t i=0; i<valvec.size(); ++i) {
        std::string valstr = StringUtility::addrToString(valvec[i]);
        col += 1 + valstr.size();
        if (col>width) {
            o <<"\n" <<prefix <<std::string(7, ' ');
            col = prefix.size()+7+1+valstr.size();
        }
        o <<" " <<valstr;
    }
    o <<"\n";

    if (has_retval)
        o <<prefix <<"retval " <<retval <<"\n";
    for (size_t i=0; i<callee_ids.size(); ++i)
        o <<prefix <<"fcall " <<callee_ids[i] <<"\n";
    for (size_t i=0; i<syscalls.size(); ++i)
        o <<prefix <<"scall " <<syscalls[i] <<"\n";
    if (fault)
        o <<prefix <<AnalysisFault::fault_name(fault) <<"\n";
    o <<prefix <<"ninsns " <<ninsns <<"\n";
}

OutputGroups::~OutputGroups()
{
    for (IdOutputMap::iterator i=ogroups.begin(); i!=ogroups.end(); ++i)
        delete i->second;
    if (file) {
        fclose(file);
        unlink(filename.c_str());
    }
}

void
OutputGroups::erase(int64_t hashkey)
{
    IdOutputMap::iterator i1 = ogroups.find(hashkey);
    if (i1!=ogroups.end()) {
        OutputGroupDesc d(i1->second);
        OutputIdMap::iterator i2 = ids.find(d);
        assert(i2!=ids.end());
        ids.erase(i2);
        delete i1->second;
        ogroups.erase(i1);
    }
}

void
OutputGroup::add_param(const std::string vtype, int pos, int64_t value)
{
    assert(!vtype.empty());
    assert(pos>=0);
    switch (vtype[0]) {
        case 'V':
            insert_value(value, pos);
            break;
        case 'R':
            insert_retval(value);
            break;
        case 'F':
            assert(fault == AnalysisFault::NONE);
            fault = (AnalysisFault::Fault)value;
            break;
        case 'C': {
            if ((size_t)pos>=callee_ids.size())
                callee_ids.resize(pos+1, 0);
            callee_ids[pos] = value;
            break;
        }
        case 'S':
            if ((size_t)pos>=syscalls.size())
                syscalls.resize(pos+1, 0);
            syscalls[pos] = value;
            break;
        case 'I':
            ninsns += value;
            break;
        default:
            assert(!"invalid output value type");
            abort();
    }
}

int64_t
OutputGroups::insert(const OutputGroup &ogroup, int64_t hashkey)
{
    bool in_database = hashkey != -1;
    if (-1==hashkey)
        hashkey = generate_hashkey();

    // Save in memory
    OutputGroup *ogroup_ptr = new OutputGroup(ogroup);
    ogroups[hashkey] = ogroup_ptr;
    OutputGroupDesc desc(ogroup_ptr);
    ids.insert(std::make_pair(desc, hashkey));

    // Save to file
    if (!in_database) {
        if (NULL==file) {
            char tpl[64];
            strcpy(tpl, "/tmp/roseXXXXXX");
            int fd = mkstemp(tpl);
            file = fdopen(fd, "wb");
            assert(fileno(file)==fd);
            filename = tpl;
        }
        int status = 0; // sign bit will be set on failure; other bits are meaningless
        std::vector<OutputGroup::value_type> valvec = ogroup.get_values();
        for (size_t i=0; i<valvec.size(); ++i) {
            unsigned val = valvec[i];
            status |= fprintf(file, "%"PRId64",V,%zu,%u\n", hashkey, i, val);
        }
        const std::vector<int> &callee_ids = ogroup.get_callee_ids();
        for (size_t i=0; i<callee_ids.size(); ++i)
            status |= fprintf(file, "%"PRId64",C,%zu,%"PRIu64"\n", hashkey, i, (uint64_t)callee_ids[i]);
        const std::vector<int> &syscalls = ogroup.get_syscalls();
        for (size_t i=0; i<syscalls.size(); ++i)
            status |= fprintf(file, "%"PRId64",S,%zu,%d\n", hashkey, i, syscalls[i]);
        if (ogroup.get_fault()!=AnalysisFault::NONE)
            status |= fprintf(file, "%"PRId64",F,0,%d\n", hashkey, (int)ogroup.get_fault());
        if (ogroup.get_retval().first)
            status |= fprintf(file, "%"PRId64",R,0,%"PRIu64"\n", hashkey, (uint64_t)ogroup.get_retval().second);
        status |= fprintf(file, "%"PRId64",I,0,%zu\n", hashkey, ogroup.get_ninsns());
        if (status<0) {
            std::cerr <<"CloneDetection::OutputGroups::insert: write failed for " <<filename <<"\n";
            abort();
        }
    }

    return hashkey;
}

int64_t
OutputGroups::find(const OutputGroup &ogroup) const
{
    OutputGroupDesc desc(&ogroup);
    OutputIdMap::const_iterator found = ids.find(desc);
    return found==ids.end() ? -1 : found->second;
}

bool
OutputGroups::load(const SqlDatabase::TransactionPtr &tx, int64_t hashkey)
{
    SqlDatabase::StatementPtr stmt = tx->statement("select vtype, pos, val"
                                                   " from semantic_outputvalues"
                                                   " where hashkey = ?"
                                                   " order by hashkey, pos")
                                     ->bind(0, hashkey);
    OutputGroup ogroup;
    erase(hashkey);
    size_t nrows=0;
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row, ++nrows)
        ogroup.add_param(row.get<std::string>(0), row.get<int>(1), row.get<int64_t>(2));
    if (0==nrows)
        return false;
    insert(ogroup, hashkey);
    return true;
}

void
OutputGroups::load(const SqlDatabase::TransactionPtr &tx)
{
    ogroups.clear();
    ids.clear();

    SqlDatabase::StatementPtr stmt = tx->statement("select hashkey, vtype, pos, val"
                                                   " from semantic_outputvalues"
                                                   " order by hashkey, pos");
    int64_t prev_hashkey = -1;
    OutputGroup ogroup;
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        int64_t hashkey = row.get<int64_t>(0);
        std::string vtype= row.get<std::string>(1);
        int pos = row.get<int>(2);
        int64_t value = row.get<int64_t>(3);

        if (hashkey!=prev_hashkey) {
            if (prev_hashkey>=0)
                insert(ogroup, prev_hashkey);
            prev_hashkey = hashkey;
            ogroup = OutputGroup();
        }

        ogroup.add_param(vtype, pos, value);

    }
    if (prev_hashkey>0)
        insert(ogroup, prev_hashkey);
}

int64_t
OutputGroups::generate_hashkey()
{
    int64_t hashkey = time(NULL);
    for (size_t i=0; i<7; ++i)
        hashkey ^= lcg();
    return hashkey & 0x7fffffffffffffffull;
}

void
OutputGroups::save(const SqlDatabase::TransactionPtr &tx)
{
    if (file) {
        fclose(file);
        file = NULL;
        std::ifstream in(filename.c_str());
        tx->bulk_load("semantic_outputvalues", in);
        unlink(filename.c_str());
    }
}

std::vector<int64_t>
OutputGroups::hashkeys() const
{
    std::vector<int64_t> retval;
    retval.reserve(ogroups.size());
    for (IdOutputMap::const_iterator i=ogroups.begin(); i!=ogroups.end(); ++i)
        retval.push_back(i->first);
    return retval;
}

bool
OutputGroups::exists(int64_t hashkey) const
{
    return ogroups.find(hashkey)!=ogroups.end();
}

const OutputGroup *
OutputGroups::lookup(int64_t hashkey) const
{
    IdOutputMap::const_iterator found = ogroups.find(hashkey);
    return found==ogroups.end() ? NULL : found->second;
}

/*******************************************************************************************************************************
 *                                      Instruction coverage
 *******************************************************************************************************************************/

void
InsnCoverage::execute(SgAsmInstruction *insn)
{
    rose_addr_t insn_va = insn->get_address();
    CoverageMap::iterator found = coverage.find(insn_va);
    if (found==coverage.end()) {
        assert(func_id>=0 && igroup_id>=0);
        coverage.insert(std::make_pair(insn_va, InsnCoverageRow(func_id, igroup_id, insn_va, coverage.size(), 1)));
    } else {
        ++found->second.nhits;
    }
}

size_t
InsnCoverage::total_ninsns() const
{
    size_t retval = 0;
    for (CoverageMap::const_iterator ci=coverage.begin(); ci!=coverage.end(); ++ci)
        retval += ci->second.nhits;
    return retval;
}

void
InsnCoverage::flush(const SqlDatabase::TransactionPtr &tx)
{
    for (CoverageMap::iterator ci=coverage.begin(); ci!=coverage.end(); ++ci) {
        if (ci->second.nhits > ci->second.nhits_saved) {
            InsnCoverageRow tmp = ci->second;
            tmp.nhits -= tmp.nhits_saved;
            insert(tmp);
            ci->second.nhits_saved = ci->second.nhits;
        }
    }
    WriteOnlyTable<InsnCoverageRow>::flush(tx);
}

double
InsnCoverage::get_ratio(SgAsmFunction *func, int func_id, int igroup_id) const
{
    struct: AstSimpleProcessing {
        Disassembler::AddressSet addrs;
        void visit(SgNode *node) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(node))
                addrs.insert(insn->get_address());
        }
    } c;
    c.traverse(func, preorder);
    if (c.addrs.empty())
        return 1.0;
    size_t denominator = c.addrs.size();
    for (CoverageMap::const_iterator ci=coverage.begin(); ci!=coverage.end(); ++ci) {
        if (ci->second.func_id==func_id && ci->second.igroup_id==igroup_id)
            c.addrs.erase(ci->first);
    }
    return 1.0 - (double)c.addrs.size() / denominator;
}


void
InsnCoverage::get_instructions(std::vector<SgAsmInstruction*>& insns, SgAsmInterpretation* interp, SgAsmFunction* top)
{
    // Find the instructions
    std::vector<SgAsmX86Instruction*> tmp_insns;
    InstructionMap instr_map = interp->get_instruction_map(interp);
    if (top != NULL) {
        FindInstructionsVisitor vis;
        AstQueryNamespace::querySubTree(top, std::bind2nd(vis, &tmp_insns));

        // Addresses and map of instructions in the subtree of top
        std::vector<rose_addr_t> query_insns_addr;
        for (std::vector<SgAsmX86Instruction*>::iterator it = tmp_insns.begin(); it != tmp_insns.end(); ++it)
            query_insns_addr.push_back((*it)->get_address());
        std::sort(query_insns_addr.begin(), query_insns_addr.end());

        // Addresses of instruction in the trace
        std::vector<rose_addr_t> trace_insns_addr;
        for (CoverageMap::iterator it = coverage.begin(); it != coverage.end(); ++it)
            trace_insns_addr.push_back(it->first);
        std::sort(trace_insns_addr.begin(), trace_insns_addr.end());

        // Addresses in the intersection of the trace and the subtree of top
        std::vector<rose_addr_t> trace_query_intersection(trace_insns_addr.size() > query_insns_addr.size() ?
                                                          trace_insns_addr.size() : query_insns_addr.size());
        std::vector<rose_addr_t>::iterator intersection_end;
        intersection_end = std::set_intersection(query_insns_addr.begin(), query_insns_addr.end(), trace_insns_addr.begin(),
                                                 trace_insns_addr.end(), trace_query_intersection.begin());

        //Instructions
        Disassembler::InstructionMap::iterator find_it;
        for (std::vector<rose_addr_t>::iterator  it = trace_query_intersection.begin(); it != intersection_end; ++it) {
            find_it = instr_map.find(*it);
            if (find_it == instr_map.end()) {
                assert(!"element not found in instruction map");
                abort();
            }
            insns.push_back(find_it->second);
        }
    } else {
        Disassembler::InstructionMap::iterator find_it;
        for (CoverageMap::iterator it = coverage.begin(); it != coverage.end(); ++it) {
            find_it = instr_map.find(it->first);
            if (find_it == instr_map.end()) {
                assert(!"element not found in instruction map");
                abort();
            }
            insns.push_back(find_it->second);
        }
    }
}

/*******************************************************************************************************************************
 *                                      Dynamic Call Graph
 *******************************************************************************************************************************/

void
DynamicCallGraph::call(int caller_id, int callee_id)
{
    assert(func_id>=0 && igroup_id>=0);
    assert(caller_id>=0 && callee_id>=0);
    if (func_id==last_call.func_id && igroup_id==last_call.igroup_id &&
        caller_id==last_call.caller_id && callee_id==last_call.callee_id) {
        ++last_call.ncalls;
        if (keep_in_memory)
            rows.back() = last_call;
    } else {
        if (last_call.caller_id>=0)
            insert(last_call);
        last_call = DynamicCallGraphRow(func_id, igroup_id, caller_id, callee_id, call_sequence++);
        if (keep_in_memory)
            rows.push_back(last_call);
    }
}

void
DynamicCallGraph::flush(const SqlDatabase::TransactionPtr &tx)
{
    if (last_call.caller_id>=0) {
        insert(last_call);
        last_call = DynamicCallGraphRow();
    }
    WriteOnlyTable<DynamicCallGraphRow>::flush(tx);
}



/*******************************************************************************************************************************
 *                                      Miscellaneous functions
 *******************************************************************************************************************************/


SgProject *
open_specimen(const SqlDatabase::TransactionPtr &tx, FilesTable &files, int specimen_id, const std::string &argv0)
{
    // Perform dynamic linking if we linked with something other than our own builtins.so.
    bool do_link = 0 < (tx->statement("select count(*)"
                                      "  from semantic_specfiles as specfile"
                                      "  join semantic_files as file"
                                      "    on specfile.file_id=file.id and file.name not like '%/builtins.so'"
                                      "  where specimen_id = ?")
                        ->bind(0, specimen_id)->execute_int());
    if (NULL==open_specimen(files.name(specimen_id), argv0, do_link))
        return NULL; // no interpretation
    return SageInterface::getProject();
}

SgAsmInterpretation *
open_specimen(const std::string &specimen_name, const std::string &argv0, bool do_link)
{
    // Parse the binary container (ELF, PE, etc) but do not disassemble yet.
    std::cerr <<argv0 <<": parsing binary container: " <<specimen_name <<"\n";
    std::string arg = "-rose:read_executable_file_format_only";
    char *argv[4];
    argv[0] = strdup(argv0.c_str());
    argv[1] = strdup(arg.c_str());
    argv[2] = strdup(specimen_name.c_str());
    argv[3] = NULL;
    int argc = 3;
    SgProject *project = frontend(argc, argv);
    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    // Find the primary interpretation (e.g., the PE, not DOS, interpretation in PE files).
    std::cerr <<argv0 <<": finding primary interpretation\n";
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    if (interps.empty()) {
        std::cerr <<argv0 <<": ERROR: no binary specimen given\n";
        return NULL;
    }
    SgAsmInterpretation *interp = interps.back();
    SgAsmGenericHeader *spec_header = interp->get_headers()->get_headers().back();

    // If the specimen is a shared library then the .text section (and most of the others) are mapped starting at virtual
    // address zero.  This will interfere with tests where the specimen reads from a memory address that was randomly
    // generated--because the randomly generated address will be a low number. When the test tries to read from that low
    // address it will read an instruction rather than a value from an input queue. The way we avoid this is to pre-map the low
    // addresses to force BinaryLoader::remap() to move the specimen to a higher address. [Robb P. Matzke 2013-11-26]
    AddressInterval exclusion_area = AddressInterval::baseSize(0, 0x03000000); // size is arbitrary, but something recognizable
    SgAsmGenericSection *text = spec_header->get_section_by_name(".text");
    bool added_exclusion_area = false;
    if (text!=NULL && text->is_mapped()) {
        Extent textExtent = text->get_mapped_preferred_extent();
        AddressInterval textInterval = AddressInterval::hull(textExtent.first(), textExtent.last());
        if (textInterval.isOverlapping(exclusion_area)) {
            std::cerr <<argv0 <<": specimen is a shared object; remapping it to a higher virtual address\n";
            MemoryMap *map = interp->get_map();
            if (!map)
                interp->set_map(map = new MemoryMap);
            map->insert(exclusion_area,
                        MemoryMap::Segment::anonymousInstance(exclusion_area.size(), 0, "temporary exclusion area"));
            added_exclusion_area = true;
        }
    }

    // Get the shared libraries, map them, and apply relocation fixups. We have to do the mapping step even if we're not
    // linking with shared libraries, because that's what gets the various file sections lined up in memory for the
    // disassembler.
    SgAsmGenericHeader *builtin_header = NULL;
    if (do_link)
        std::cerr <<argv0 <<": loading shared libraries\n";
    if (BinaryLoader *loader = BinaryLoader::lookup(interp)) {
        try {
            loader = loader->clone(); // so our settings are private
            if (do_link) {
                // Link with the standard libraries
                loader->add_directory("/lib32");
                loader->add_directory("/usr/lib32");
                loader->add_directory("/lib");
                loader->add_directory("/usr/lib");
                if (char *ld_library_path = getenv("LD_LIBRARY_PATH")) {
                    std::vector<std::string> paths;
                    StringUtility::splitStringIntoStrings(ld_library_path, ':', paths/*out*/);
                    loader->add_directories(paths);
                }
                loader->link(interp);
            } else if (isSgAsmElfFileHeader(spec_header)) {
                // If we didn't link with the standard C library, then link with our own library.  Our own library is much
                // smaller and is intended to provide the same semantics as the C library for those few functions that GCC
                // occassionally inlines because the function is built into GCC.  This allows us to compare non-optimized
                // code (without these functions having been inlined) with optimized code (where these functions are inlined)
                // because the unoptimized code will traverse into the definitions we provide.
                //
                // Note: Using BinaryLoader to link our builtins.so does not actually resolve the imports in the specimen.
                // I'm not sure why, but we work around this by calling link_builtins() below. [Robb P. Matzke 2013-07-11]
                std::string subdir = "/projects/BinaryCloneDetection/semantic";
                loader->add_directory(ROSE_AUTOMAKE_TOP_BUILDDIR + subdir);
                loader->add_directory(ROSE_AUTOMAKE_TOP_SRCDIR + subdir);
                loader->add_directory(ROSE_AUTOMAKE_LIBDIR);
                std::string builtin_name = loader->find_so_file("builtins.so");
                SgBinaryComposite *binary = SageInterface::getEnclosingNode<SgBinaryComposite>(interp);
                assert(binary!=NULL);
                SgAsmGenericFile *builtin_file = loader->createAsmAST(binary, builtin_name);
                assert(builtin_file!=NULL);
                builtin_header = builtin_file->get_headers()->get_headers().back();
            }
            loader->remap(interp);
            BinaryLoader::FixupErrors fixup_errors;
            loader->fixup(interp, &fixup_errors);
            if (!fixup_errors.empty()) {
                std::cerr <<argv0 <<": warning: " <<fixup_errors.size()
                          <<" relocation fixup error" <<(1==fixup_errors.size()?"":"s") <<" encountered\n";
            }
            if (SageInterface::querySubTree<SgAsmInterpretation>(project).size() != interps.size())
                std::cerr <<argv0 <<": warning: new interpretations created by the linker; mixed 32- and 64-bit libraries?\n";
        } catch (const BinaryLoader::Exception &e) {
            std::cerr <<argv0 <<": BinaryLoader error: " <<e.what() <<"\n";
            return NULL;
        }
    } else {
        std::cerr <<argv0 <<": ERROR: no suitable loader/linker found\n";
        return NULL;
    }
    assert(interp->get_map()!=NULL);
    if (added_exclusion_area)
        interp->get_map()->erase(exclusion_area);
    MemoryMap map = *interp->get_map();
    link_builtins(spec_header, builtin_header, &map);

    // Figure out what to disassemble.  If we did dynamic linking then we can mark the .got and .got.plt sections as read-only
    // because we've already filled them in with the addresses of the dynamically linked entities.  This will allow the
    // disassembler to know the successors for the indirect JMP instruction in the .plt section (the dynamic function thunks).
    if (do_link) {
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
            SgAsmGenericSectionPtrList sections = (*hi)->get_sections_by_name(".got.plt");      // ELF
            SgAsmGenericSectionPtrList s2 = (*hi)->get_sections_by_name(".got");                // ELF
            SgAsmGenericSectionPtrList s3 = (*hi)->get_sections_by_name(".import");             // PE
            sections.insert(sections.end(), s2.begin(), s2.end());
            sections.insert(sections.end(), s3.begin(), s3.end());
            for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); si!=sections.end(); ++si) {
                if ((*si)->is_mapped()) {
                    map.at((*si)->get_mapped_actual_va())
                       .limit((*si)->get_mapped_size())
                       .changeAccess(MemoryMap::READABLE, ~MemoryMap::READABLE);
                }
            }
        }
    }

    // Disassemble the executable
    std::cerr <<argv0 <<": disassembling and partitioning\n";
    if (Disassembler *disassembler = Disassembler::lookup(interp)) {
        disassembler = disassembler->clone(); // so our settings are private
#if 1 // FIXME [Robb P. Matzke 2013-05-14]
        // We need to handle -rose:disassembler_search, -rose:partitioner_search, and -rose:partitioner_config
        // command-line switches.
#endif
        Partitioner *partitioner = new Partitioner();
        SgAsmBlock *gblk = partitioner->partition(interp, disassembler, &map);
        interp->set_global_block(gblk);
        gblk->set_parent(interp);
    } else {
        std::cerr <<argv0 <<": unable to disassemble this specimen\n";
        return NULL;
    }
    return interp;
}

void
link_builtins(SgAsmGenericHeader *imports_header, SgAsmGenericHeader *exports_header, MemoryMap *map)
{
    using namespace StringUtility;

    // Find the addresses for the exported functions
    struct Exports: AstSimpleProcessing {
        NameAddress address;
        void visit(SgNode *node) {
            if (SgAsmElfSymbol *sym = isSgAsmElfSymbol(node)) {
                if (sym->get_def_state()==SgAsmGenericSymbol::SYM_DEFINED &&
                    sym->get_binding()==SgAsmGenericSymbol::SYM_GLOBAL &&
                    sym->get_type()==SgAsmGenericSymbol::SYM_FUNC &&
                    sym->get_bound()!=NULL &&
                    sym->get_bound()->get_name()->get_string().compare(".text")==0) {
                    std::string name = sym->get_name()->get_string();
                    SgAsmGenericSection *section = sym->get_bound();
                    assert(section->is_mapped());
                    rose_addr_t section_delta = section->get_mapped_actual_va() - section->get_mapped_preferred_va();
                    rose_addr_t va = sym->get_value() + section_delta;
                    address[name] = va;
#if 0 /*DEBUGGING [Robb P. Matzke 2013-12-13]*/
                    std::cerr <<"ROBB: exported symbol \"" <<name <<"\""
                              <<" = " <<addrToString(sym->get_value()) <<"\n"
                              <<"        bound to " <<section->get_name()->get_string(true) <<"\n"
                              <<"           preferred = " <<addrToString(section->get_mapped_preferred_va()) <<"\n"
                              <<"           actual    = " <<addrToString(section->get_mapped_actual_va()) <<"\n"
                              <<"           delta     = " <<addrToString(section->get_mapped_actual_va() -
                                                                         section->get_mapped_preferred_va()) <<"\n"
                              <<"           final     = " <<addrToString(va) <<"\n";
#endif
                }
            }
        }
    } exports;
    exports.traverse(exports_header, preorder);


    // Link the exports into the import table.  For ELF, this means processing R_386_JMP_SLOT relocations.
    struct Fixup: AstSimpleProcessing {
        SgAsmGenericHeader *imports_header;
        SgAsmElfSymbolPtrList imports;
        const Exports &exports;
        MemoryMap *map;
        BinaryLoaderElf loader;
        SgAsmElfSymbolSection *symsec;
        Fixup(SgAsmGenericHeader *imports_header, const Exports &exports, MemoryMap *map)
            : imports_header(imports_header), exports(exports), map(map) {
            if ((symsec = isSgAsmElfSymbolSection(imports_header->get_section_by_name(".dynsym")))) {
                imports = symsec->get_symbols()->get_symbols();
            }
        }
        void visit(SgNode *node) {
            if (SgAsmElfRelocEntry *reloc = isSgAsmElfRelocEntry(node)) {
                if (reloc->get_type()==SgAsmElfRelocEntry::R_386_JMP_SLOT && reloc->get_sym()<imports.size()) {
                    assert(reloc->get_sym() < imports.size());
                    SgAsmElfSymbol *import_symbol = imports[reloc->get_sym()];
                    std::string name = import_symbol->get_name()->get_string();

                    // The value to be written. This is the address of the loaded exported function.
                    rose_addr_t export_va = exports.address.get_value_or(name, 0);
                    if (export_va==0)
                        return;
#if 0 /*DEBUGGING [Robb P. Matzke 2013-12-13]*/
                    std::cerr <<"ROBB: fixup for \"" <<name <<"\""
                              <<" (symbol #" <<reloc->get_sym() <<" in \"" <<symsec->get_name()->get_string(true) <<"\")\n"
                              <<"        value to write: " <<addrToString(export_va) <<" (exported function)\n";
#endif


                    // The memory address to which we'll eventually write the address of the exported function.  For elf, we'll
                    // probably be writing the value into the .got.plt section.  Take into account that the section into which
                    // we're writing may have been moved to a different place in virtual memory than where it expected to be.
                    rose_addr_t write_va = reloc->get_r_offset();
                    SgAsmGenericSection *write_sec = loader.find_section_by_preferred_va(imports_header, write_va);
                    write_va += write_sec->get_mapped_actual_va() - write_sec->get_mapped_preferred_va();
#if 0 /*DEBUGGING [Robb P. Matzke 2013-12-13]*/
                    std::cerr <<"        where to write: \n"
                              <<"           reloc offset = " <<addrToString(reloc->get_r_offset()) <<"\n"
                              <<"           section = \"" <<write_sec->get_name()->get_string(true) <<"\"\n"
                              <<"           write va = " <<addrToString(write_va) <<"\n";
#endif

                    uint32_t export_va_le;
                    ByteOrder::host_to_le(export_va, &export_va_le);
                    size_t nwrite = map->writeQuick(&export_va_le, write_va, 4);
                    if (nwrite!=4) {
                        std::cerr <<"        write failed into memory map:\n";
                        map->dump(std::cerr, "          ");
                    }
                }
            }
        }
    } fixer_upper(imports_header, exports, map);
    fixer_upper.traverse(imports_header, preorder);
}

int64_t
start_command(const SqlDatabase::TransactionPtr &tx, int argc, char *argv[], const std::string &desc, time_t begin)
{
    int64_t hashkey = LinearCongruentialGenerator().next(63, 7);
    if (0==begin)
        begin = time(NULL);
    tx->statement("insert into semantic_history (hashkey, begin_time, notation, command) values (?, ?, ?, ?)")
        ->bind(0, hashkey)
        ->bind(1, begin)
        ->bind(2, desc)
        ->bind(3, StringUtility::join(" ", argv, argc))
        ->execute();
    return hashkey;
}

void
finish_command(const SqlDatabase::TransactionPtr &tx, int64_t hashkey, const std::string &desc)
{
    tx->statement("update semantic_history set end_time = ? where hashkey = ?")
        ->bind(0, time(NULL))
        ->bind(1, hashkey)
        ->execute();
    if (!desc.empty()) {
        tx->statement("update semantic_history set notation = ? where hashkey = ?")
            ->bind(0, desc)
            ->bind(1, hashkey)
            ->execute();
    }
}

// Return the name of a file for the specified header (or AST descendant thereof)
std::string
filename_for_header(SgAsmGenericHeader *hdr, bool basename)
{
    std::string retval;
    SgAsmGenericFile *file = SageInterface::getEnclosingNode<SgAsmGenericFile>(hdr);
    if (file!=NULL && !file->get_name().empty())
        retval = file->get_name();
    if (basename) {
        size_t slash = retval.rfind('/');
        if (slash!=std::string::npos)
            retval = retval.substr(slash+1);
    }
    return retval;
}

// Return the name of a file containing the specified function.
std::string
filename_for_function(SgAsmFunction *function, bool basename)
{
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(function);
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
        size_t nmatch;
        (*hi)->get_section_by_va(function->get_entry_va(), false, &nmatch);
        if (nmatch>0)
            return filename_for_header(*hi, basename);
    }
    return "";
}

// Return a list of functions that are not already in the database, and appropriate ID numbers.  The functions are not
// actually added yet because we don't have all the info.
IdFunctionMap
missing_functions(const SqlDatabase::TransactionPtr &tx, CloneDetection::FilesTable &files,
                  const std::vector<SgAsmFunction*> &functions)
{
    tx->execute("create temporary table tmp_functions (entry_va integer, file_id integer, pos integer)");
    SqlDatabase::StatementPtr stmt1 = tx->statement("insert into tmp_functions (entry_va, file_id, pos) values (?, ?, ?)");
    for (size_t i=0; i<functions.size(); ++i) {
        std::string filename = CloneDetection::filename_for_function(functions[i]);
        stmt1->bind(0, functions[i]->get_entry_va());
        stmt1->bind(1, files.insert(filename));
        stmt1->bind(2, i);
        stmt1->execute();
    }
    int next_id = tx->statement("select coalesce(max(id),-1)+1 from semantic_functions")->execute_int();
    SqlDatabase::StatementPtr stmt2 = tx->statement("select newfunc.entry_va, newfunc.file_id, newfunc.pos"
                                                    " from tmp_functions as newfunc"
                                                    " left join semantic_functions as oldfunc"
                                                    "   on newfunc.entry_va = oldfunc.entry_va and"
                                                    "      newfunc.file_id = oldfunc.file_id"
                                                    " where oldfunc.id is null");
    IdFunctionMap retval;
    for (SqlDatabase::Statement::iterator row=stmt2->begin(); row!=stmt2->end(); ++row) {
        size_t pos = row.get<int>(2);
        retval[next_id++] = functions[pos];
    }
    tx->execute("drop table tmp_functions");
    return retval;
}

// Returns the intersection of "functions" with the list of functions in the database.
IdFunctionMap
existing_functions(const SqlDatabase::TransactionPtr &tx, CloneDetection::FilesTable &files,
                   const std::vector<SgAsmFunction*> &functions)
{
    IdFunctionMap retval;

    // Build a map from function entry address to function
    std::map<rose_addr_t, SgAsmFunction*> addr_func;
    for (std::vector<SgAsmFunction*>::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi)
        addr_func[(*fi)->get_entry_va()] = *fi;

    // Create a table for all our functions
    tx->execute("create temporary table tmp_functions (entry_va integer, file_id integer)");
    SqlDatabase::StatementPtr stmt1 = tx->statement("insert into tmp_functions (entry_va, file_id) values (?, ?)");
    for (size_t i=0; i<functions.size(); ++i) {
        std::string filename = CloneDetection::filename_for_function(functions[i]);
        stmt1->bind(0, functions[i]->get_entry_va());
        stmt1->bind(1, files.insert(filename));
        stmt1->execute();
    }

    // Get the intersection of the database functions and our functions
    SqlDatabase::StatementPtr stmt2 = tx->statement("select func.id, func.entry_va"
                                                    " from semantic_functions as func"
                                                    " join tmp_functions as tmp"
                                                    " on func.entry_va=tmp.entry_va and func.file_id=tmp.file_id");
    for (SqlDatabase::Statement::iterator c2=stmt2->begin(); c2!=stmt2->end(); ++c2) {
        int id = c2.get<int>(0);
        rose_addr_t entry_va = c2.get<rose_addr_t>(1);
        assert(addr_func.find(entry_va)!=addr_func.end());
        retval[id] = addr_func[entry_va];
    }

    tx->execute("drop table tmp_functions");
    return retval;
}

std::string
save_binary_data(const SqlDatabase::TransactionPtr &tx, int64_t cmd_id, const std::string &filename)
{
    int fd = open(filename.c_str(), O_RDONLY);
    assert(fd>=0);
    struct stat sb;
    int status __attribute__((unused)) = fstat(fd, &sb);
    assert(status>=0);
    const uint8_t *data = (const uint8_t*)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    assert(data!=MAP_FAILED);
    close(fd);
    std::string hashkey = save_binary_data(tx, cmd_id, data, sb.st_size);
    munmap((void*)data, sb.st_size);
    return hashkey;
}

std::string
save_binary_data(const SqlDatabase::TransactionPtr &tx, int64_t cmd_id, const uint8_t *data, size_t size)
{
    std::string hashkey = Combinatorics::digest_to_string(Combinatorics::sha1_digest(data, size));
    if (tx->statement("select count(*) from semantic_binaries where hashkey = ?")->bind(0, hashkey)->execute_int())
        return hashkey; // already saved

    // Store the base64-encoded chunks
    SqlDatabase::StatementPtr stmt = tx->statement("insert into semantic_binaries"
                                                   // 0       1    2    3
                                                   "(hashkey, cmd, pos, chunk) values (?, ?, ?, ?)")
                                     ->bind(0, hashkey)
                                     ->bind(1, cmd_id);
    size_t at=0, chunk_size=10*1024*1024, chunk_number=0;
    while (at<size) {
        size_t nbytes = std::min(chunk_size, size-at);
        std::string chunk_base64 = StringUtility::encode_base64(data+at, nbytes);
        stmt->bind(2, chunk_number++);
        stmt->bind(3, chunk_base64);
        stmt->execute();
        at += nbytes;
    }

    return hashkey;
}

std::string
load_binary_data(const SqlDatabase::TransactionPtr &tx, const std::string &hashkey, std::string filename)
{
    // Create the file where the biary data will be saved
    assert(40==hashkey.size());
    assert(std::string::npos==hashkey.find_first_not_of("0123456789abcdef"));
    int fd = -1;
    if (filename.empty()) {
        char s[64];
        strcpy(s, "/tmp/roseXXXXXX");
        fd = mkstemp(s);
        filename = s;
    } else {
        fd = open(filename.c_str(), O_CREAT|O_TRUNC|O_RDWR, 0666);
    }
    assert(fd>=0);

    // Read, decode, and save the binary data.
    SqlDatabase::StatementPtr stmt = tx->statement("select chunk from semantic_binaries where hashkey = ? order by pos")
                                     ->bind(0, hashkey);
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        std::vector<uint8_t> chunk_binary = StringUtility::decode_base64(row.get<std::string>(0));
        ssize_t nwrite __attribute__((unused)) = write(fd, &chunk_binary[0], chunk_binary.size());
        assert((size_t)nwrite==chunk_binary.size());
    }

    // Verify that the checksum is correct
    struct stat sb;
    int status __attribute__((unused)) = fstat(fd, &sb);
    assert(status>=0);
    const uint8_t *data = (const uint8_t*)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    assert(data!=MAP_FAILED);
    std::string digest = Combinatorics::digest_to_string(Combinatorics::sha1_digest(data, sb.st_size));
    munmap((void*)data, sb.st_size);
    assert(0==digest.compare(hashkey));
    close(fd);

    return filename;
}

std::string
save_ast(const SqlDatabase::TransactionPtr &tx, int64_t cmd_id)
{
    char filename[64];
    sprintf(filename, "/tmp/roseXXXXXX");
    int fd = mkstemp(filename);
    assert(fd>=0);
    close(fd);
    AST_FILE_IO::startUp(SageInterface::getProject());
    AST_FILE_IO::writeASTToFile(filename);
    std::string hashkey = save_binary_data(tx, cmd_id, filename);
    unlink(filename);
    return hashkey;
}

SgProject *
load_ast(const SqlDatabase::TransactionPtr &tx, const std::string &hashkey)
{
    std::string filename = load_binary_data(tx, hashkey);
    AST_FILE_IO::clearAllMemoryPools();
    SgProject *project = AST_FILE_IO::readASTFromFile(filename);
    unlink(filename.c_str());
    assert(project==SageInterface::getProject());
    return project;
}

std::string
function_to_str(SgAsmFunction *function, const FunctionIdMap &ids)
{
    std::ostringstream ss;
    FunctionIdMap::const_iterator idi = ids.find(function);
    std::string func_name = function->get_name();
    std::string file_name = filename_for_function(function);

    ss <<StringUtility::addrToString(function->get_entry_va());

    bool printed = false;
    if (!func_name.empty()) {
        ss <<" <\"" <<func_name <<"\"";
        printed = true;
    }
    if (idi!=ids.end()) {
        ss <<(printed?" ":" <") <<"id=" <<idi->second;
        printed = true;
    }
    if (!file_name.empty()) {
        ss <<(printed?" ":" <") <<"in " <<file_name;
        printed = true;
    }
    if (printed)
        ss <<">";

    return ss.str();
}

static double
function_returns_value(const FuncAnalysis &fa, size_t ncalls_s, size_t nretused_s)
{
    if (ncalls_s>0) {
        assert(nretused_s <= ncalls_s);
        return (double)nretused_s/ncalls_s;
    } else {
        assert(fa.nretused<=fa.ncalls);
        assert(fa.nvoids<=fa.ntests);
        if (0==fa.ncalls && 0==fa.ntests)
            return 0.5;

        double p1 = fa.ntests>0 ? 1.0-(double)fa.nvoids/fa.ntests : 0.5;
        double p1_weight = fa.ntests>0 ? 1.0 : 0.0;
        double p2 = fa.ncalls>0 ? (double)fa.nretused/fa.ncalls : 0.5;
        double p2_weight = fa.ncalls>0 ? 1.0 : 0.0;

        if (fa.ntests>0 && fa.nvoids==fa.ntests) {
            if (fa.nretused>0) {
                // The function was tested but never wrote to EAX, but callers read a return value. Something bizarre is going
                // on here! Maybe the function has a logic error?  Give weight to the callers with the assumption that
                // something in our testing may have prevented writing to EAX.
                p1_weight = 0.0;
                p2_weight = 1.0;
            } else {
                // Never wrote to EAX, and no caller read from EAX (or there were no callers).
                return 0.0;
            }
        } else if (0==fa.ntests) {
            // The function was never tested, so we don't know if it would write to EAX.  Our only choice is to rely entirely
            // on whether the callers read a return value.  If there were no callers then we know nothing.
            if (0==fa.ncalls)
                return 0.5;
            p1_weight = 0.0;
            p2_weight = 1.0;
        } else if (0==fa.ncalls) {
            // The function was never called.  Even functions that write to EAX might only be using it as a temporary. If some
            // of the tests don't write to EAX then EAX is probably not a return value.
            if (fa.nvoids>0)
                p1 *= 0.25;
            p1_weight = 1.0;
            p2_weight = 0.0;
        } else {
            // The function was tested (and writes to EAX at least once), and it was called. Since even void functions can
            // write to EAX as a temporary, we give more weight to whether the callers read a return value.
            p1_weight = 1.0;
            p2_weight = 5.0;
        }

        assert(p1_weight+p2_weight > 0);
        return (p1*p1_weight+p2*p2_weight)/(p1_weight+p2_weight);
    }
}

double
function_returns_value(const SqlDatabase::TransactionPtr &tx, int func_id)
{

    SqlDatabase::StatementPtr stmt1 = tx->statement("select sum(ncalls), sum(nretused), sum(ntests), sum(nvoids)"
                                                    " from semantic_funcpartials where func_id = ? group by func_id")
                                      ->bind(0, func_id);
    SqlDatabase::Statement::iterator row = stmt1->begin();
    FuncAnalysis fa;
    if (row!=stmt1->end()) {
        fa.ncalls = row.get<size_t>(0);
        fa.nretused = row.get<size_t>(1);
        fa.ntests = row.get<size_t>(2);
        fa.nvoids = row.get<size_t>(3);
    }

    SqlDatabase::StatementPtr stmt2 = tx->statement("select callsites, retvals_used from semantic_functions"
                                                    " where id = ?")->bind(0, func_id);
    row = stmt2->begin();
    assert(row!=stmt2->end());
    size_t ncalls_s = row.get<size_t>(0);
    size_t nretused_s = row.get<size_t>(1);

    return function_returns_value(fa, ncalls_s, nretused_s);
}

std::map<int, double>
function_returns_value(const SqlDatabase::TransactionPtr &tx)
{
    FuncAnalyses fas;
    std::map<int, double> retval;
    SqlDatabase::StatementPtr stmt1 = tx->statement("select sum(ncalls), sum(nretused), sum(ntests), sum(nvoids), func_id"
                                                   " from semantic_funcpartials group by func_id");
    for (SqlDatabase::Statement::iterator row=stmt1->begin(); row!=stmt1->end(); ++row) {
        int func_id = row.get<int>(4);
        FuncAnalysis &fa = fas[func_id];
        fa.ncalls = row.get<size_t>(0);
        fa.nretused = row.get<size_t>(1);
        fa.ntests = row.get<size_t>(2);
        fa.nvoids = row.get<size_t>(3);
    }

    SqlDatabase::StatementPtr stmt2 = tx->statement("select id, callsites, retvals_used from semantic_functions");
    for (SqlDatabase::Statement::iterator row=stmt2->begin(); row!=stmt2->end(); ++row) {
        int func_id = row.get<int>(0);
        FuncAnalysis &fa = fas[func_id];
        size_t ncalls_s = row.get<size_t>(1);
        size_t nretused_s = row.get<size_t>(2);
        retval[func_id] = function_returns_value(fa, ncalls_s, nretused_s);
    }
    return retval;
}

bool
function_returns_value_p(const SqlDatabase::TransactionPtr &tx, int func_id, double threshold)
{
    return function_returns_value(tx, func_id) >= threshold;
}

} // namespace
