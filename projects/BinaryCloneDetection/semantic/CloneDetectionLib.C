#include "sage3basic.h"
#include "AST_FILE_IO.h"
#include "CloneDetectionLib.h"
#include "Combinatorics.h"
#include "BinaryLoader.h"
#include "Partitioner.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

using namespace sqlite3x;

namespace CloneDetection {

void
OutputGroup::add_param(const std::string vtype, int pos, int64_t value)
{
    assert(!vtype.empty());
    assert(pos>=0);
    switch (vtype[0]) {
        case 'V':
            assert(pos>=0 && (size_t)pos==values.size());
            add_value(value);
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

bool
OutputGroup::operator<(const OutputGroup &other) const
{
    typedef std::pair<Values::const_iterator, Values::const_iterator> vi_pair;
    typedef std::pair<std::vector<int>::const_iterator, std::vector<int>::const_iterator> ii_pair;

    // scalar comparisons */
    if (fault != other.fault)
        return fault < other.fault;
    if (ninsns != other.ninsns)
        return ninsns < other.ninsns;

    // Values
    if (values.size() != other.values.size())
        return values.size() < other.values.size();
    vi_pair vi = std::mismatch(values.begin(), values.end(), other.values.begin());
    if (vi.first!=values.end())
        return *(vi.first) < *(vi.second);

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

    // They must be equal
    assert(*this==other);
    return false;
}

bool
OutputGroup::operator==(const OutputGroup &other) const
{
    return (values.size()==other.values.size() &&
            std::equal(values.begin(), values.end(), other.values.begin()) &&
            callee_ids.size()==other.callee_ids.size() &&
            std::equal(callee_ids.begin(), callee_ids.end(), other.callee_ids.begin()) &&
            syscalls.size()==other.syscalls.size() &&
            std::equal(syscalls.begin(), syscalls.end(), other.syscalls.begin()) &&
            fault == other.fault &&
            ninsns == other.ninsns);
}

void
OutputGroup::clear()
{
    values.clear();
    callee_ids.clear();
    syscalls.clear();
    fault = AnalysisFault::NONE;
    ninsns = 0;
}

void
OutputGroup::print(std::ostream &o, const std::string &title, const std::string &prefix) const
{
    if (!title.empty())
        o <<title <<"\n";
    for (Values::const_iterator vi=values.begin(); vi!=values.end(); ++vi)
        o <<prefix <<"value " <<*vi <<"\n";
    for (size_t i=0; i<callee_ids.size(); ++i)
        o <<prefix <<"fcall " <<callee_ids[i] <<"\n";
    for (size_t i=0; i<syscalls.size(); ++i)
        o <<prefix <<"scall " <<syscalls[i] <<"\n";
    if (fault)
        o <<prefix <<AnalysisFault::fault_name(fault) <<"\n";
    o <<prefix <<"ninsns " <<ninsns <<"\n";
}

void
OutputGroup::print(RTS_Message *m, const std::string &title, const std::string &prefix) const
{
    if (m && m->get_file()) {
        std::ostringstream ss;
        print(ss, title, prefix);
        m->mesg("%s", ss.str().c_str());
    }
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
            strcpy(tpl, "./outputvalues-XXXXXX");
            int fd = mkstemp(tpl);
            file = fdopen(fd, "wb");
            assert(fileno(file)==fd);
            filename = tpl;
        }
        int status = 0; // sign bit will be set on failure; other bits are meaningless
        int pos = 0;
        for (OutputGroup::Values::const_iterator vi=ogroup.values.begin(); vi!=ogroup.values.end(); ++vi)
            status |= fprintf(file, "%"PRId64",V,%d,%u\n", hashkey, pos++, *vi);
        for (size_t i=0; i<ogroup.callee_ids.size(); ++i)
            status |= fprintf(file, "%"PRId64",C,%zu,%d\n", hashkey, i, ogroup.callee_ids[i]);
        for (size_t i=0; i<ogroup.syscalls.size(); ++i)
            status |= fprintf(file, "%"PRId64",S,%zu,%d\n", hashkey, i, ogroup.syscalls[i]);
        if (ogroup.fault!=AnalysisFault::NONE)
            status |= fprintf(file, "%"PRId64",F,0,%d\n", hashkey, (int)ogroup.fault);
        status |= fprintf(file, "%"PRId64",I,0,%zu\n", hashkey, ogroup.ninsns);
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
    int nchars = round((double)n/total * WIDTH);
    std::string bar(nchars, '=');
    bar += std::string(WIDTH-nchars, ' ');
    if (!mesg.empty()) {
        assert(WIDTH>9);
        size_t mesg_sz = std::min((size_t)WIDTH-9, mesg.size());
        std::string s = mesg_sz>=mesg.size() ? mesg : mesg.substr(0, mesg_sz)+"...";
        bar.replace(3, s.size(), s);
    }
    std::ostringstream ss;
    ss <<" " <<std::setw(3) <<(int)round(100.0*n/total) <<"% " <<cur <<"/" <<total <<" |" <<bar <<"|";
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

void
FilesTable::load(const SqlDatabase::TransactionPtr &tx)
{
    clear();
    SqlDatabase::StatementPtr stmt = tx->statement("select id, name from semantic_files");
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        int id = row.get<int>(0);
        std::string name = row.get<std::string>(1);
        rows[id] = Row(id, name, true);
        name_idx[name] = id;
        next_id = std::max(next_id, id+1);
    }
}

void
FilesTable::save(const SqlDatabase::TransactionPtr &tx)
{
    SqlDatabase::StatementPtr stmt = tx->statement("insert into semantic_files (id, name) values (?, ?)");
    for (Rows::iterator ri=rows.begin(); ri!=rows.end(); ++ri) {
        assert(ri->second.id==ri->first);
        if (!ri->second.in_db) {
            stmt->bind(0, ri->first);
            stmt->bind(1, ri->second.name);
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

int
FilesTable::insert(const std::string &name)
{
    NameIdx::iterator found = name_idx.find(name);
    if (found!=name_idx.end())
        return found->second;
    int id = next_id++;
    rows[id] = Row(id, name, false);
    name_idx[name] = id;
    return id;
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

SgAsmInterpretation *
open_specimen(const std::string &specimen_name, const std::string &argv0, bool do_link)
{
    // Parse the binary container (ELF, PE, etc) but do not disassemble yet.
    std::cerr <<argv0 <<": parsing binary container\n";
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

    // Get the shared libraries, map them, and apply relocation fixups. We have to do the mapping step even if we're not
    // linking with shared libraries, because that's what gets the various file sections lined up in memory for the
    // disassembler.
    if (do_link)
        std::cerr <<argv0 <<": loading shared libraries\n";
    if (BinaryLoader *loader = BinaryLoader::lookup(interp)) {
        try {
            loader = loader->clone(); // so our settings are private
            if (do_link) {
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
            std::cerr <<argv0 <<": BinaryLoader error: " <<e.mesg <<"\n";
            return NULL;
        }
    } else {
        std::cerr <<argv0 <<": ERROR: no suitable loader/linker found\n";
        return NULL;
    }

    // Figure out what to disassemble.  If we did dynamic linking then we can mark the .got and .got.plt sections as read-only
    // because we've already filled them in with the addresses of the dynamically linked entities.  This will allow the
    // disassembler to know the successors for the indirect JMP instruction in the .plt section (the dynamic function thunks).
    assert(interp->get_map()!=NULL);
    MemoryMap map = *interp->get_map();
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
                    Extent mapped_va((*si)->get_mapped_actual_va(), (*si)->get_mapped_size());
                    map.mprotect(mapped_va, MemoryMap::MM_PROT_READ, true/*relax*/);
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

// Return the name of a file containing the specified function.
std::string
filename_for_function(SgAsmFunction *function, bool basename)
{
    std::string retval;
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(function);
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
        size_t nmatch;
        (*hi)->get_section_by_va(function->get_entry_va(), false, &nmatch);
        if (nmatch>0) {
            SgAsmGenericFile *file = SageInterface::getEnclosingNode<SgAsmGenericFile>(*hi);
            if (file!=NULL && !file->get_name().empty()) {
                retval = file->get_name();
                break;
            }
        }
    }
    if (basename) {
        size_t slash = retval.rfind('/');
        if (slash!=std::string::npos)
            retval = retval.substr(slash+1);
    }
    return retval;
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

void
save_ast(const SqlDatabase::TransactionPtr &tx, int specimen_file_id, int64_t cmd_id)
{
    AST_FILE_IO::startUp(SageInterface::getProject());
    std::string s = AST_FILE_IO::writeASTToString();
    std::vector<uint8_t> s_binary(s.begin(), s.end());
    std::string s_base64 = StringUtility::encode_base64(s_binary);
    tx->statement("delete from semantic_ast where file_id = ?")
        ->bind(0, specimen_file_id)
        ->execute();
    tx->statement("insert into semantic_ast (file_id, content, cmd) values (?, ?, ?)")
        ->bind(0, specimen_file_id)
        ->bind(1, s_base64)
        ->bind(2, cmd_id)
        ->execute();
}

SgProject *
load_ast(const SqlDatabase::TransactionPtr &tx, int specimen_file_id)
{
    SqlDatabase::StatementPtr stmt = tx->statement("select content from semantic_ast where file_id = ?");
    stmt->bind(0, specimen_file_id);
    SqlDatabase::Statement::iterator row = stmt->begin();
    if (row==stmt->end())
        return NULL;
    std::string s_base64 = row.get<std::string>(0);
    std::vector<uint8_t> s_binary = StringUtility::decode_base64(s_base64);
    std::string s(s_binary.begin(), s_binary.end());
    ++row;
    assert(row==stmt->end()); // table should have only one row for the AST

    AST_FILE_IO::clearAllMemoryPools();
    SgProject *project = AST_FILE_IO::readASTFromString(s);
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

bool
InputGroup::load(const SqlDatabase::TransactionPtr &tx, int igroup_id)
{
    clear();
    SqlDatabase::StatementPtr stmt = tx->statement("select vtype, val"
                                                   " from semantic_inputvalues"
                                                   " where id = ?"
                                                   " order by vtype, pos");
    stmt->bind(0, igroup_id);
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        std::string vtype = row.get<std::string>(0);
        assert(!vtype.empty());
        uint64_t val = row.get<uint64_t>(1);
        switch (vtype[0]) {
            case 'I':
                add_integer(val);
                break;
            case 'P':
                add_pointer(val);
                break;
            default:
                assert(!"unknown input value type"); abort();
        }
    }

    return size()!=0;
}



        
        



} // namespace
