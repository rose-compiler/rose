#include "rose.h"
#include "RSIM_Private.h"

// Define this if you want LOTS of debugging output.  The default is to only print enough messages to show progress.
// #define CLONE_DETECTOR_VERBOSE

// This source file can only be compiled if the simulator is enabled. Furthermore, it only makes sense to compile this file if
// the simulator is using our code from CloneDetectionSemantics.h (i.e., the CloneDetection.patch file has been applied to
// RSIM).
#if defined(ROSE_ENABLE_SIMULATOR) && defined(RSIM_CloneDetectionSemantics_H)

#include "RSIM_Linux32.h"
#include "BinaryPointerDetection.h"
#include "YicesSolver.h"
#include "DwarfLineMapper.h"

// We'd like to not have to depend on a particular relational database management system, but ROSE doesn't have
// support for anything like ODBC.  The only other options are either to generate SQL output, which precludes being
// able to perform queries, or using sqlite3x.h, which is distributed as part of ROSE.  If you want to use a RDMS other
// than SQLite3, then dump SQL from the generated SQLite3 database and load it into whatever RDMS you want.
#include "sqlite3x.h"
using namespace sqlite3x; // its top-level class name start with "sqlite3_"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>
#include <boost/algorithm/string/regex.hpp>


typedef std::set<SgAsmFunction*> Functions;
typedef std::map<SgAsmFunction*, int> FunctionIdMap;
typedef CloneDetection::InputValues InputValues;
typedef std::set<uint32_t> OutputValues;

static void usage(const std::string &arg0, int exit_status)
{
    size_t slash = arg0.rfind('/');
    std::string basename = slash==std::string::npos ? arg0 : arg0.substr(slash+1);
    if (0==basename.substr(0, 3).compare("lt-"))
        basename = basename.substr(3);
    std::cerr <<"usage: " <<basename <<" [--database=STR] [--nfuzz=N] [--ninputs=N1,N2] [--max-insns=N]"
              <<" [SIMULATOR_SWITCHES] SPECIMEN [SPECIMEN_ARGS...]\n"
              <<"   --database=STR\n"
              <<"                     Specifies the name of the database into which results are placed.\n"
              <<"                     The default is \"clones.db\".\n"
              <<"   --nfuzz=N[,START]\n"
              <<"                     Number of times to fuzz test each function (default 10) and the\n"
              <<"                     sequence number of the first test (default 0).\n"
              <<"   --ninputs=N1[,N2]\n"
              <<"                     Number of input values to supply each time a function is run. When\n"
              <<"                     N1 and N2 are both specified, then N1 is the number of pointers and\n"
              <<"                     N2 is the number of non-pointers.  When only N1 is specified it will\n"
              <<"                     indicate the number of pointers and the number of non-pointers. The\n"
              <<"                     default is three pointers and three non-pointers.  If a function\n"
              <<"                     requires more input values, then null/zero are supplied to it.\n"
              <<"   --min-function-size=N\n"
              <<"                     Minimum size of functions to analyze. Any function containing fewer\n"
              <<"                     than N instructions is not processed. The default is to analyze all\n"
              <<"                     functions.\n"
              <<"   --max-insns=N\n"
              <<"                     Maximum number of instructions to simulate per function. The default\n"
              <<"                     is 256.\n"
              <<"   --verbose\n"
              <<"                     Show lots of diagnostics if the --debug simulator switch is specified.\n"
              <<"                     The default (with --debug) is to show only enough output to track the\n"
              <<"                     analysis progress.  Without --debug, hardly any diagnostics are\n"
              <<"                     produced.\n";
    exit(exit_status);
}

// Command-line switches
struct Switches {
    Switches()
        : dbname("clones.db"), firstfuzz(0), nfuzz(10), npointers(3), nnonpointers(3),
          min_funcsz(0), max_insns(256), verbose(false) {}
    std::string dbname;                 /**< Name of database in which to store results. */
    size_t firstfuzz;                   /**< Sequence number for starting fuzz test. */
    size_t nfuzz;                       /**< Number of times to run each function, each time with a different input sequence. */
    size_t npointers, nnonpointers;     /**< Number of pointer and non-pointer values to supply to as inputs per fuzz test. */
    size_t min_funcsz;                  /**< Minimum function size measured in instructions; skip smaller functions. */
    size_t max_insns;                   /**< Maximum number of instrutions per fuzz test before giving up. */
    bool verbose;                       /**< Produce lots of output?  Traces each instruction as it is simulated. */
};

// Special output values:
#define FAULT_NONE        0
#define FAULT_DISASSEMBLY 911000001    // disassembly failed possibly due to bad address
#define FAULT_INSN_LIMIT  911000002    // maximum number of instructions executed
#define FAULT_HALT        911000003    // x86 HLT instruction executed
#define FAULT_INTERRUPT   911000004    // x86 INT instruction executed
#define FAULT_SEMANTICS   911000005    // some fatal problem with instruction semantics, such as a not-handled instruction
#define FAULT_SMTSOLVER   911000006    // some fault in the SMT solver

// Return the short name of a fault ID
static const char *
fault_name(int fault)
{
    switch (fault) {
        case FAULT_NONE: return "";
        case FAULT_DISASSEMBLY: return "FAULT_DISASSEMBLY";
        case FAULT_INSN_LIMIT:  return "FAULT_INSN_LIMIT";
        case FAULT_HALT:        return "FAULT_HALT";
        case FAULT_INTERRUPT:   return "FAULT_INTERRUPT";
        case FAULT_SEMANTICS:   return "FAULT_SEMANTICS";
        case FAULT_SMTSOLVER:   return "FAULT_SMTSOLVER";
        default:
            assert(!"fault not handled");
            abort();
    }
}

/*******************************************************************************************************************************
 *                                      Clone Detector
 *******************************************************************************************************************************/

/** Main driving function for clone detection.  This is the class that chooses inputs, runs each function, and looks at the
 *  outputs to decide how to partition the functions.  It does this repeatedly in order to build a PartitionForest. The
 *  analyze() method is the main entry point. */
class CloneDetector {
protected:
    static const char *name;            /**< For --debug output. */
    RSIM_Thread *thread;                /**< Thread where analysis is running. */
    sqlite3_connection sqlite;          /**< Database in which to place results; may already exist */
    std::vector<OutputValues> output_sets; /**< Distinct sets of output values. */
    Switches opt;                       /**< Analysis configuration switches from the command line. */

public:
    CloneDetector(RSIM_Thread *thread, const Switches &opt): thread(thread), opt(opt) {
        open_db(opt.dbname);
    }

    void open_db(const std::string &dbname) {
        sqlite.open(dbname.c_str());
        sqlite.busy_timeout(15*60*1000); // 15 minutes
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);

        // Execute the SQL that describes the schema.  We used to just pipe this into the sqlite3 command, but we had
        // problems when running in parallel.  It would be nice to be able to pass the whole string to executenonquery(),
        // but that seems to execute only the first statement in the string.  Therefore, we split the big, multi-line string
        // into statements and execute each individually.
        extern const char *clone_detection_schema; // contents of Schema.sql file
        std::string stmts(clone_detection_schema);
        boost::regex stmt_re("( ( '([^']|'')*'   )"     // string literal
                             "| ( --[^\n]*       )"     // comment
                             "| ( [^;]           )"     // other
                             ")+ ;", boost::regex::perl|boost::regex::mod_x);
        typedef boost::algorithm::find_iterator<std::string::iterator> Sfi; // string find iterator
        for (Sfi i=make_find_iterator(stmts, boost::algorithm::regex_finder(stmt_re)); i!=Sfi(); ++i) {
            std::string stmt = boost::copy_range<std::string>(*i);
            try {
                sqlite.executenonquery(stmt);
            } catch (const database_error &e) {
                std::cerr <<stmt <<"\n";
                throw;
            }
        }

        // Populate the semantic_faults table.
        struct FaultInserter {
            FaultInserter(sqlite3_connection &db, int id, const char *name, const char *desc) {
                sqlite3_command cmd1(db, "select count(*) from semantic_faults where id = ?");
                cmd1.bind(1, id);
                if (cmd1.executeint()==0) {
                    sqlite3_command cmd2(db, "insert into semantic_faults (id, name, desc) values (?,?,?)");
                    cmd2.bind(1, id);
                    cmd2.bind(2, name);
                    cmd2.bind(3, desc);
                    cmd2.executenonquery();
                }
            }
        };
#define add_fault(DB, ID, DESC) FaultInserter(DB, ID, #ID, DESC)
        add_fault(sqlite, FAULT_DISASSEMBLY, "disassembly failed");
        add_fault(sqlite, FAULT_INSN_LIMIT,  "simulation instruction limit reached");
        add_fault(sqlite, FAULT_HALT,        "x86 HLT instruction executed");
        add_fault(sqlite, FAULT_INTERRUPT,   "interrupt or x86 INT instruction executed");
        add_fault(sqlite, FAULT_SEMANTICS,   "instruction semantics error");
        add_fault(sqlite, FAULT_SMTSOLVER,   "SMT solver error");
#undef  add_fault

        lock.commit();
    }

    // Allocate a page of memory in the process address space.
    rose_addr_t allocate_page(rose_addr_t hint=0) {
        RSIM_Process *proc = thread->get_process();
        rose_addr_t addr = proc->mem_map(hint, 4096, MemoryMap::MM_PROT_RW, MAP_ANONYMOUS, 0, -1);
        assert((int64_t)addr>=0 || (int64_t)addr<-256); // disallow error numbers
        return addr;
    }

    // Obtain a memory map for disassembly
    MemoryMap *disassembly_map(RSIM_Process *proc) {
        MemoryMap *map = new MemoryMap(proc->get_memory(), MemoryMap::COPY_SHALLOW);
        map->prune(MemoryMap::MM_PROT_READ); // don't let the disassembler read unreadable memory, else it will segfault

        // Removes execute permission for any segment whose debug name does not contain the name of the executable. When
        // comparing two different executables for clones, we probably don't need to compare code that came from dynamically
        // linked libraries since they will be identical in both executables.
        struct Pruner: MemoryMap::Visitor {
            std::string exename;
            Pruner(const std::string &exename): exename(exename) {}
            virtual bool operator()(const MemoryMap*, const Extent&, const MemoryMap::Segment &segment_) {
                MemoryMap::Segment *segment = const_cast<MemoryMap::Segment*>(&segment_);
                if (segment->get_name().find(exename)==std::string::npos) {
                    unsigned p = segment->get_mapperms();
                    p &= ~MemoryMap::MM_PROT_EXEC;
                    segment->set_mapperms(p);
                }
                return true;
            }
        } pruner(proc->get_exename());
        map->traverse(pruner);
        return map;
    }

    // Get all the functions defined for this process image.  We do this by disassembling the entire process executable memory
    // and using CFG analysis to figure out where the functions are located.
    Functions find_functions(RTS_Message *m, RSIM_Process *proc) {
        m->mesg("%s: disassembling entire specimen image...\n", name);
        MemoryMap *map = disassembly_map(proc);
        if (opt.verbose) {
            std::ostringstream ss;
            map->dump(ss, "  ");
            m->mesg("%s: using this memory map for disassembly:\n%s", name, ss.str().c_str());
        }
        SgAsmBlock *gblk = proc->disassemble(false/*take no shortcuts*/, map);
        delete map; map=NULL;

        // Save the disassembly listing for future reference
        std::string listing_name = proc->get_exename() + ".lst";
        m->mesg("%s: saving program listing in %s\n", name, listing_name.c_str());
        std::ofstream of(listing_name.c_str());
        AsmUnparser().unparse(of, gblk);
        of.close();

        // All functions, but prune away things we don't care about
        std::vector<SgAsmFunction*> functions = SageInterface::querySubTree<SgAsmFunction>(gblk);
        m->mesg("%s: found %zu function%s", name, functions.size(), 1==functions.size()?"":"s");
        for (std::vector<SgAsmFunction*>::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            size_t ninsns = SageInterface::querySubTree<SgAsmInstruction>(*fi).size();
            if (opt.min_funcsz>0 && ninsns < opt.min_funcsz) {
                if (opt.verbose)
                    m->mesg("%s:   skipping function 0x%08"PRIx64" because it has only %zu instruction%s\n",
                            name, (*fi)->get_entry_va(), ninsns, 1==ninsns?"":"s");
                *fi = NULL;
            }
        }
        functions.erase(std::remove(functions.begin(), functions.end(), (SgAsmFunction*)NULL), functions.end());
        m->mesg("%s: %zu function%s left after pruning", name, functions.size(), 1==functions.size()?"":"s");
        return Functions(functions.begin(), functions.end());
    }

    // Perform a pointer-detection analysis on the specified function. We'll need the results in order to determine whether a
    // function input should consume a pointer or a non-pointer from the input value set.
    CloneDetection::PointerDetector* detect_pointers(RTS_Message *m, RSIM_Thread *thread, SgAsmFunction *func) {
        // Choose an SMT solver. This is completely optional.  Pointer detection still seems to work fairly well (and much,
        // much faster) without an SMT solver.
        SMTSolver *solver = NULL;
#if 0   // optional code
        if (YicesSolver::available_linkage())
            solver = new YicesSolver;
#endif
        CloneDetection::InstructionProvidor *insn_providor = new CloneDetection::InstructionProvidor(thread->get_process());
        m->mesg("%s: %s pointer detection analysis", name, function_to_str(func).c_str());
        CloneDetection::PointerDetector *pd = new CloneDetection::PointerDetector(insn_providor, solver);
        pd->initial_state().registers.gpr[x86_gpr_sp] = SYMBOLIC_VALUE<32>(thread->policy.INITIAL_STACK);
        pd->initial_state().registers.gpr[x86_gpr_bp] = SYMBOLIC_VALUE<32>(thread->policy.INITIAL_STACK);
        //pd.set_debug(stderr);
        try {
            pd->analyze(func);
        } catch (...) {
            // probably the instruction is not handled by the semantics used in the analysis.  For example, the
            // instruction might be a floating point instruction that isn't handled yet.
            m->mesg("%s: %s pointer analysis FAILED", name, function_to_str(func).c_str());
        }
        if (opt.verbose && m->get_file()) {
            const CloneDetection::PointerDetector::Pointers plist = pd->get_pointers();
            for (CloneDetection::PointerDetector::Pointers::const_iterator pi=plist.begin(); pi!=plist.end(); ++pi) {
                std::ostringstream ss;
                if (pi->type & BinaryAnalysis::PointerAnalysis::DATA_PTR)
                    ss <<"data ";
                if (pi->type & BinaryAnalysis::PointerAnalysis::CODE_PTR)
                    ss <<"code ";
                ss <<"pointer at " <<pi->address;
                m->mesg("   %s", ss.str().c_str());
            }
        }
        return pd;
    }

    // Get the ID for a file, adding a new entry to the table if necessary.
    // Must aquire the write lock before calling this function.
    int get_file_id(const std::string &filename) {
        sqlite3_command cmd1(sqlite, "select coalesce(max(id),-1) from semantic_files where name = ?");
        sqlite3_command cmd2(sqlite,
                             "insert into semantic_files (id, name)"
                             "values ((select coalesce(max(id),-1)+1 from semantic_files), ?)");

        cmd1.bind(1, filename);
        int file_id = cmd1.executeint();
        if (file_id < 0) {
            cmd2.bind(1, filename);
            cmd2.executenonquery();
            file_id = cmd1.executeint();
            assert(file_id>=0);
        }
        return file_id;
    }

    struct FuncStats {
        FuncStats(): file_id(-1), isize(0), dsize(0), size(0), ninsns(0) {}
        FuncStats(size_t isize, size_t dsize, size_t size, size_t ninsns)
            : file_id(-1), isize(isize), dsize(dsize), size(size), ninsns(ninsns) {}
        int file_id;
        size_t isize, dsize, size, ninsns;
    };


    // Save each function to the database.  Returns a mapping from function object to ID number.
    // We have to do this in a database-efficient manner without holding a transaction lock too long because we're
    // likely to be running in parallel.
    FunctionIdMap save_functions(const Functions &functions) {
        typedef std::map<rose_addr_t/*entry_va*/, SgAsmFunction*> AddrFunc;
        typedef std::map<SgAsmFunction*, FuncStats> Stats;

        struct InstructionSelector: SgAsmFunction::NodeSelector {
            virtual bool operator()(SgNode *node) { return isSgAsmInstruction(node)!=NULL; }
        } iselector;

        struct DataSelector: SgAsmFunction::NodeSelector {
            virtual bool operator()(SgNode *node) { return isSgAsmStaticData(node)!=NULL; }
        } dselector;

        FunctionIdMap retval;
        RTS_Message *m = thread->tracing(TRACE_MISC);

        // Pre-compute some function info that doesn't depend on the database.
        m->mesg("%s:   calculating function information from AST", name);
        AddrFunc addrfunc;
        Stats stats;
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            SgAsmFunction *func = *fi;
            ExtentMap e_insns, e_data, e_total;
            size_t ninsns = func->get_extent(&e_insns, NULL, NULL, &iselector);
            func->get_extent(&e_data,  NULL, NULL, &dselector);
            func->get_extent(&e_total);
            stats[func] = FuncStats(e_insns.size(), e_data.size(), e_total.size(), ninsns);
            addrfunc[func->get_entry_va()] = func;
        }

        // Populate the semantic_files table (assuming our functions can come from multiple files)
        m->mesg("%s:   populating file table from binary function info", name);
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi)
            stats[*fi].file_id = get_file_id(filename_for_function(*fi));
        lock.commit();

        // Scan the whole function table and keep only those entries that correspond to our functions.  Since our functions
        // might come from multiple files, there isn't an easy way to limit the query to only those files (we could use "in"
        // but this seems about as fast).
        m->mesg("%s:   getting function IDs from the database", name);
        sqlite3_command cmd1(sqlite, "select id, entry_va, file_id from semantic_functions");
        lock.begin(sqlite3_transaction::LOCK_IMMEDIATE);
        sqlite3_reader c1 = cmd1.executereader();
        while (c1.read()) {
            int function_id = c1.getint(0);
            rose_addr_t entry_va = (unsigned)c1.getint(1);
            int file_id = c1.getint(2);

            AddrFunc::iterator found = addrfunc.find(entry_va);
            if (found==addrfunc.end())
                continue;
            SgAsmFunction *func = found->second;
            if (file_id!=stats[func].file_id)
                continue;
            retval[func] = function_id;
        }
        lock.commit();

        // Figure out which functions are not in the database.  By time we get done with this loop some other process might
        // have added those functions, but we'll handle that later.
        Functions to_add;
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            SgAsmFunction *func = *fi;
            if (retval.find(func)==retval.end())
                to_add.insert(func);
        }

        // Add necessary functions to the semantic_functions table, but be careful because some other process might have added
        // them since we released the lock above.
        m->mesg("%s:   adding %zu function ID%s to the database", name, to_add.size(), 1==to_add.size()?"":"s");
        sqlite3_command cmd2(sqlite, "select coalesce(max(id),-1) from semantic_functions where entry_va=? and file_id=?");
        sqlite3_command cmd3(sqlite, "insert into semantic_functions"
                             // 1   2         3         4        5      6      7     8
                             " (id, entry_va, funcname, file_id, isize, dsize, size, ninsns)"
                             " values (?,?,?,?,?,?,?,?)");
        lock.begin(sqlite3_transaction::LOCK_IMMEDIATE);
        int next_id = sqlite.executeint("select coalesce(max(id),-1)+1 from semantic_functions");
        Functions added;
        for (Functions::iterator fi=to_add.begin(); fi!=to_add.end(); ++fi) {
            SgAsmFunction *func = *fi;
            cmd2.bind(1, func->get_entry_va());
            cmd2.bind(2, stats[func].file_id);
            int func_id = cmd2.executeint();
            if (func_id<0) {
                retval[func] = next_id;
                cmd3.bind(1, next_id);
                cmd3.bind(2, func->get_entry_va());
                cmd3.bind(3, func->get_name());
                cmd3.bind(4, stats[func].file_id);
                const FuncStats &s = stats[func];
                cmd3.bind(5, s.isize);
                cmd3.bind(6, s.dsize);
                cmd3.bind(7, s.size);
                cmd3.bind(8, s.ninsns);
                cmd3.executenonquery();
                added.insert(func);
                ++next_id;
            }
        }
        lock.commit();
        m->mesg("%s:   added %zu function ID%s to the database", name, added.size(), 1==added.size()?"":"s");

        // For each function we added to the database, also add the assembly listing. We assume that if the function already
        // exists in the database then its assembly listing also exists, but this might not be true if the process is
        // interruped here.
        sqlite3_command cmd4(sqlite,
                             "insert into semantic_instructions"
                             // 1        2     3         4            5         6            7
                             " (address, size, assembly, function_id, position, src_file_id, src_line)"
                             " values (?,?,?,?,?,?,?)");
        m->mesg("%s:   computing address-to-source mapping", name);
        BinaryAnalysis::DwarfLineMapper dlm(thread->get_process()->get_project());
        dlm.fix_holes();
        AsmUnparser unparser;
        unparser.staticDataDisassembler.init(thread->get_process()->get_disassembler());
        m->mesg("%s:   saving assembly code for each function", name);
        for (Functions::iterator ai=added.begin(); ai!=added.end(); ++ai) {
            SgAsmFunction *func = *ai;
            int func_id = retval[func];
            if (opt.verbose)
                std::cerr <<"  unparsing function " <<func_id;
            std::string function_lst = unparser.to_string(func);
            std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(func);

            // much faster when using transactions, but don't hold lock too long; i.e., lock inside this loop, not outside
            sqlite3_transaction lock2(sqlite, sqlite3_transaction::LOCK_IMMEDIATE, sqlite3_transaction::DEST_COMMIT);

            for (size_t i=0; i<insns.size(); ++i) {
                BinaryAnalysis::DwarfLineMapper::SrcInfo loc = dlm.addr2src(insns[i]->get_address());
                int src_file_id = loc.file_id < 0 ? -1 : get_file_id(Sg_File_Info::getFilenameFromID(loc.file_id));
                cmd4.bind(1, insns[i]->get_address());
                cmd4.bind(2, insns[i]->get_size());
                cmd4.bind(3, unparseInstructionWithAddress(insns[i]));
                cmd4.bind(4, func_id);
                cmd4.bind(5, i);
                cmd4.bind(6, src_file_id);
                cmd4.bind(7, loc.line_num);
                cmd4.executenonquery();
                if (opt.verbose)
                    std::cerr <<'.';
            }
            if (opt.verbose)
                std::cerr <<"\n";
        }

        return retval;
    }

    // Returns true if the file looks like text
    bool is_text_file(FILE *f) {
        if (!f)
            return false;
        char buf[4096];
        fpos_t pos;
        if (fgetpos(f, &pos)<0)
            return false;
        size_t nread = fread(buf, 1, sizeof buf, f);
        if (0==nread)
            return false; // empty files are binary
        int status = fsetpos(f, &pos);
        assert(status>=0);
        for (size_t i=0; i<nread; ++i)
            if (!isascii(buf[i]))
                return false;
        return true;
    }

    // Suck source code into the database.  For any file that can be read and which does not have lines already saved
    // in the semantic_sources table, read each line of the file and store them in semantic_sources.
    void save_files() {
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE, sqlite3_transaction::DEST_COMMIT);
        sqlite3_command cmd1(sqlite,
                             "select files.id, files.name"
                             " from semantic_files as files"
                             " left join semantic_sources as sources"
                             " on files.id = sources.file_id"
                             " where sources.file_id is null");
        sqlite3_command cmd2(sqlite, "select count(*) from semantic_sources where file_id = ?");
        sqlite3_command cmd3(sqlite, "insert into semantic_sources (file_id, linenum, line) values (?,?,?)");

        sqlite3_reader c1 = cmd1.executereader();
        while (c1.read()) {
            int file_id = c1.getint(0);
            std::string file_name = c1.getstring(1);
            FILE *f = fopen(file_name.c_str(), "r");
            if (is_text_file(f)) {
                cmd2.bind(1, file_id);
                if (cmd2.executeint()<=0) {
                    if (opt.verbose)
                        std::cerr <<"  saving source code for " <<file_name;
                    char *line = NULL;
                    size_t linesz=0, line_num=0;
                    ssize_t nread;
                    while ((nread=getline(&line, &linesz, f))>0) {
                        while (nread>0 && isspace(line[nread-1]))
                            line[--nread] = '\0';
                        cmd3.bind(1, file_id);
                        cmd3.bind(2, ++line_num);
                        cmd3.bind(3, line);
                        cmd3.executenonquery();
                        if (opt.verbose)
                            std::cerr <<".";
                    }
                    if (line)
                        free(line);
                }
                if (opt.verbose)
                    std::cerr <<"\n";
            }
            if (f)
                fclose(f);
        }
    }
    
    // Choose input values for fuzz testing.  The input values come from the database if they exist there, otherwise they are
    // chosen randomly and written to the database. The set will consist of some number of non-pointers and pointers. The
    // non-pointer values are chosen randomly, but limited to a certain range.  The pointers are chosen randomly to be null or
    // non-null and the non-null values each have one page allocated via simulated mmap() (i.e., the non-null values themselves
    // are not actually random).
    InputValues choose_inputs(size_t inputset_id) {
        CloneDetection::InputValues inputs;

        // Hold write lock while we check for input values and either read or create them. Otherwise some other
        // process might concurrently decide that input values don't exist and we'll have two processes trying to create
        // the input values that might differ.
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);

        sqlite3_command cmd1(sqlite, "select vtype, val from semantic_inputvalues where id = ? order by pos");
        cmd1.bind(1, inputset_id);
        sqlite3_reader cursor = cmd1.executereader();
        while (cursor.read()) {
            std::string vtype = cursor.getstring(0);
            assert(vtype.size()>=1);
            if ('P'==vtype[0]) {
                rose_addr_t va = cursor.getint64(1);
                inputs.add_pointer(va);
            } else {
                assert('N'==vtype[0]);
                size_t val = cursor.getint64(1);
                inputs.add_integer(val);
            }
        }

        // If we didn't get any input values, then create some and add them to the database.
        if (inputs.get_integers().size() + inputs.get_pointers().size() == 0) {
            sqlite3_command cmd3(sqlite, "insert into semantic_inputvalues (id, vtype, pos, val) values (?,?,?,?)");
            static unsigned integer_modulus = 256;  // arbitrary;
            static unsigned nonnull_denom = 3;      // probability of a non-null pointer is 1/N
            for (size_t i=0; i<opt.nnonpointers; ++i) {
                uint64_t val = rand() % integer_modulus;
                inputs.add_integer(val);
                cmd3.bind(1, inputset_id);
                cmd3.bind(2, "N");
                cmd3.bind(3, i);
                cmd3.bind(4, val);
                cmd3.executenonquery();
            }
            for (size_t i=0; i<opt.npointers; ++i) {
                uint64_t val = rand()%nonnull_denom ? 0 : allocate_page();
                inputs.add_pointer(val);
                cmd3.bind(1, inputset_id);
                cmd3.bind(2, "P");
                cmd3.bind(3, i);
                cmd3.bind(4, val);
                cmd3.executenonquery();
            }
        }

        lock.commit();
        return inputs;
    }

    // Save output values into the database.  If an output set already exists in the database with these values, return that
    // set's ID instead of saving an new, identical set.
    size_t save_outputs(const OutputValues &outputs) {
        RTS_Message *m = thread->tracing(TRACE_MISC);

        // We need a write lock for the duration, otherwise some other process might determine that an output set
        // doesn't exist and then try to create the same one we're about to create.
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);

        // Load output sets from the database if they've changed since last time we loaded.  If we're the only process
        // creating output sets then this we'll only load the output sets when we first start.  We're assuming that no
        // process is deleting output sets and that sets are numbered consecutively starting at zero.
        if ((int)output_sets.size() != sqlite.executeint("select coalesce(max(id),-1)+1 from semantic_outputvalues")) {
            if (opt.verbose)
                m->mesg("%s: loading output sets from the database", name);
            sqlite3_command cmd1(sqlite, "select id, val from semantic_outputvalues");
            sqlite3_reader cursor = cmd1.executereader();
            while (cursor.read()) {
                size_t setid = cursor.getint(0);
                int value = cursor.getint(1);
                if (setid>=output_sets.size())
                    output_sets.resize(setid+1);
                output_sets[setid].insert(value);
            }
        }

        // Find an existing output set that matches the given values
        for (size_t i=0; i<output_sets.size(); ++i) {
            if (output_sets[i].size()==outputs.size() && std::equal(outputs.begin(), outputs.end(), output_sets[i].begin()))
                return i;
        }

        // Save this output set
        int setid = sqlite.executeint("select coalesce(max(id),0)+1 from semantic_outputvalues");
        output_sets.resize(setid+1);
        output_sets[setid] = outputs;
        for (OutputValues::const_iterator oi=outputs.begin(); oi!=outputs.end(); ++oi) {
            sqlite3_command cmd2(sqlite, "insert into semantic_outputvalues (id, val) values (?,?)");
            cmd2.bind(1, setid);
            cmd2.bind(2, *oi);
            cmd2.executenonquery();
        }

        lock.commit();
        return setid;
    }

    // Return the name of a file containing the specified function.  We parse the name from the mmap area.
    std::string filename_for_function(SgAsmFunction *function, bool basename=true) {
        rose_addr_t va = function->get_entry_va();
        const MemoryMap &mm = thread->get_process()->get_memory();
        if (!mm.exists(va))
            return "";
        std::string s = mm.at(va).second.get_name();
        size_t ltparen = s.find('(');
        size_t rtparen = s.rfind(')');
        std::string retval;
        if (0==s.substr(0, 5).compare("mmap(")) {
            if (rtparen==std::string::npos) {
                retval = s.substr(5);
            } else {
                assert(rtparen>=5);
                retval = s.substr(5, rtparen-5);
            }
        } else if (ltparen!=std::string::npos) {
            retval = s.substr(0, ltparen);
        }

        if (basename) {
            size_t slash = retval.rfind('/');
            if (slash!=std::string::npos)
                retval = retval.substr(slash+1);
        }
        return retval;
    }

    // Display string for function.  Includes function address, and in angle brackets, the function and file names if known.
    std::string function_to_str(SgAsmFunction *function) {
        std::ostringstream ss;
        ss <<StringUtility::addrToString(function->get_entry_va()) <<" <" <<function->get_name();
        std::string s = filename_for_function(function);
        if (!s.empty()) {
            if (function->get_name().empty())
                ss <<"\"\"";
            ss <<" in " <<s;
        }
        ss <<">";
        return ss.str();
    }
    
    // Analyze a single function by running it with the specified inputs and collecting its outputs. */
    CloneDetection::Outputs<RSIM_SEMANTICS_VTYPE> *fuzz_test(SgAsmFunction *function, CloneDetection::InputValues &inputs,
                                                             const CloneDetection::PointerDetector *pointers) {
        RSIM_Process *proc = thread->get_process();
        RTS_Message *m = thread->tracing(TRACE_MISC);
        int fault = FAULT_NONE;

        // Not sure if saving/restoring memory state is necessary. I don't think machine memory is adjusted by the semantic
        // policy's writeMemory() or readMemory() operations after the policy is triggered to enable our analysis.  But it
        // shouldn't hurt to save/restore anyway, and it's fast. [Robb Matzke 2013-01-14]
        proc->mem_transaction_start(name);
        pt_regs_32 saved_regs = thread->get_regs();

        // Trigger the analysis, resetting it to start executing the specified function using the input values and pointer
        // variable addresses we selected previously.
        thread->policy.trigger(function->get_entry_va(), &inputs, pointers);

        // "Run" the function using our semantic policy.  The function will not "run" in the normal sense since: since our
        // policy has been triggered, memory access, function calls, system calls, etc. will all operate differently.  See
        // CloneDetectionSemantics.h and CloneDetectionTpl.h for details.
        try {
            thread->main();
        } catch (const Disassembler::Exception &e) {
            // Probably due to the analyzed function's RET instruction, but could be from other things as well. In any case, we
            // stop analyzing the function when this happens.
            if (thread->policy.FUNC_RET_ADDR==e.ip) {
                if (opt.verbose)
                    m->mesg("%s: function returned", name);
                fault = FAULT_NONE;
            } else {
                if (opt.verbose)
                    m->mesg("%s: function disassembly failed at 0x%08"PRIx64": %s", name, e.ip, e.mesg.c_str());
                fault = FAULT_DISASSEMBLY;
            }
        } catch (const CloneDetection::InsnLimitException &e) {
            // The analysis might be in an infinite loop, such as when analyzing "void f() { while(1); }"
            if (opt.verbose)
                m->mesg("%s: %s", name, e.mesg.c_str());
            fault = FAULT_INSN_LIMIT;
        } catch (const RSIM_Semantics::InnerPolicy<>::Halt &e) {
            // The x86 HLT instruction appears in some functions (like _start) as a failsafe to terminate a process.  We need
            // to intercept it and terminate only the function analysis.
            if (opt.verbose)
                m->mesg("%s: function executed HLT instruction at 0x%08"PRIx64, name, e.ip);
            fault = FAULT_HALT;
        } catch (const RSIM_Semantics::InnerPolicy<>::Interrupt &e) {
            // The x86 INT instruction was executed but the policy does not know how to handle it.
            if (opt.verbose)
                m->mesg("%s: function executed INT 0x%x at 0x%08"PRIx64, name, e.inum, e.ip);
            fault = FAULT_INTERRUPT;
        } catch (const RSIM_SEMANTICS_POLICY::Exception &e) {
            // Some exception in the policy, such as division by zero.
            if (opt.verbose)
                m->mesg("%s: %s", name, e.mesg.c_str());
            fault = FAULT_SEMANTICS;
        } catch (const SMTSolver::Exception &e) {
            // Some exception in the SMT solver
            if (opt.verbose)
                m->mesg("%s: %s", name, e.mesg.c_str());
            fault = FAULT_SMTSOLVER;
        }
        
        // Gather the function's outputs before restoring machine state.
        CloneDetection::Outputs<RSIM_SEMANTICS_VTYPE> *outputs = thread->policy.get_outputs(opt.verbose!=0);
        outputs->fault = fault;
        thread->init_regs(saved_regs);
        proc->mem_transaction_rollback(name);
        return outputs;
    }

    // Detect functions that are semantically similar by running multiple iterations of partition_functions().
    void analyze() {
        thread->policy.verbose = opt.verbose!=0;
        thread->policy.max_ninsns = opt.max_insns;
        thread->set_do_coredump(false);
        thread->set_show_exceptions(false);
        RTS_Message *m = thread->tracing(TRACE_MISC);
        m->mesg("Starting clone detection analysis...");
        m->mesg("%s: database=%s, fuzz=@%zu+%zu, npointers=%zu, nnonpointers=%zu, max_insns=%zu",
                name, opt.dbname.c_str(), opt.firstfuzz, opt.nfuzz, opt.npointers, opt.nnonpointers, opt.max_insns);
        Functions functions = find_functions(m, thread->get_process());
        m->mesg("%s: saving function information and assembly listings...", name);
        FunctionIdMap func_ids = save_functions(functions);
        m->mesg("%s: saving source code listings for each function...", name);
        save_files();

        typedef std::map<SgAsmFunction*, CloneDetection::PointerDetector*> PointerDetectors;
        PointerDetectors pointers;
        sqlite3_command cmd1(sqlite, //                 1        2            3             4             5
                             "insert into semantic_fio (func_id, inputset_id, outputset_id, elapsed_time, cpu_time)"
                             " values (?,?,?,?,?)");
        sqlite3_command cmd2(sqlite, "select count(*) from semantic_fio where func_id=? and inputset_id=? limit 1");
        for (size_t fuzz_number=opt.firstfuzz; fuzz_number<opt.firstfuzz+opt.nfuzz; ++fuzz_number) {
            InputValues inputs = choose_inputs(fuzz_number);
            if (opt.verbose) {
                m->mesg("####################################################################################################");
                m->mesg("%s: fuzz testing %zu function%s with inputset %zu",
                        name, functions.size(), 1==functions.size()?"":"s", fuzz_number);
                m->mesg("%s: using these input values:\n%s", name, inputs.toString().c_str());
            }
            for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
                SgAsmFunction *func = *fi;
                inputs.reset();

                // Did we already test this function in a previous incarnation?
                cmd2.bind(1, func_ids[func]);
                cmd2.bind(2, fuzz_number);
                if (cmd2.executeint()>0) {
                    m->mesg("%s: %s fuzz test #%zu SKIPPED", name, function_to_str(func).c_str(), fuzz_number);
                    continue;
                }

                // Get the results of pointer analysis.  We could have done this before any fuzz testing started, but by doing
                // it here we only need to do it for functions that are actually tested.
                PointerDetectors::iterator ip = pointers.find(func);
                if (ip==pointers.end())
                    ip = pointers.insert(std::make_pair(func, detect_pointers(m, thread, func))).first;
                assert(ip!=pointers.end());

                // Run the test
                timeval start_time, stop_time;
                clock_t start_ticks = clock();
                gettimeofday(&start_time, NULL);
                m->mesg("%s: %s fuzz test #%zu", name, function_to_str(func).c_str(), fuzz_number);
                CloneDetection::Outputs<RSIM_SEMANTICS_VTYPE> *outputs = fuzz_test(func, inputs, ip->second);
                if (opt.verbose) {
                    std::ostringstream output_values_str;
                    OutputValues output_values = outputs->get_values(false/*exl. fault*/);
                    for (OutputValues::iterator ovi=output_values.begin(); ovi!=output_values.end(); ++ovi)
                        output_values_str <<" " <<*ovi;
                    if (outputs->fault)
                        output_values_str <<" " <<fault_name(outputs->fault);
                    m->mesg("%s: %s fuzz test #%zu output values are {%s }",
                            name, function_to_str(func).c_str(), fuzz_number, output_values_str.str().c_str());
                }
                gettimeofday(&stop_time, NULL);
                clock_t stop_ticks = clock();
                double elapsed_time = (stop_time.tv_sec - start_time.tv_sec) +
                                      ((double)stop_time.tv_usec - start_time.tv_usec) * 1e-6;

                // If clock_t is a 32-bit unsigned value then it will wrap around once every ~71.58 minutes. We expect clone
                // detection to take longer than that, so we need to be careful.
                double cpu_time = start_ticks <= stop_ticks ?
                                  (double)(stop_ticks-start_ticks) / CLOCKS_PER_SEC :
                                  (pow(2.0, 8*sizeof(clock_t)) - (start_ticks-stop_ticks)) / CLOCKS_PER_SEC;

                // Save the results. Some other process might have tested this function concurrently, in which case we'll
                // defer to the other process' results.
                size_t outputset_id = save_outputs(outputs->get_values());
                sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE, sqlite3_transaction::DEST_COMMIT);
                if (cmd2.executeint()>0) {
                    m->mesg("%s: %s fuzz test #%zu ALREADY PRESENT", name, function_to_str(func).c_str(), fuzz_number);
                } else {
                    cmd1.bind(1, func_ids[func]);
                    cmd1.bind(2, fuzz_number);
                    cmd1.bind(3, outputset_id);
                    cmd1.bind(4, elapsed_time);
                    cmd1.bind(5, cpu_time);
                    cmd1.executenonquery();
                }
            }
        }
        m->mesg("%s: final results stored in %s", name, opt.dbname.c_str());
    }
};

const char *CloneDetector::name = "CloneDetector";

/******************************************************************************************************************************/

// Instruction callback to trigger clone detection.  The specimen executable is allowed to run until it hits the specified
// trigger address (in order to cause dynamically linked libraries to be loaded and linked), at which time a CloneDetector
// object is created and thrown.
class Trigger: public RSIM_Callbacks::InsnCallback {
protected:
    rose_addr_t trigger_va;             /**< Address at which a CloneDetector should be created and thrown. */
    bool armed;                         /**< Should this object be monitoring instructions yet? */
    bool triggered;                     /**< Has this been triggered yet? */
    Switches opt;                       /**< Analysis configuration from command-line switches. */
public:
    Trigger(const Switches &opt): trigger_va(0), armed(false), triggered(false), opt(opt) {}

    // For simplicity, all threads, processes, and simulators will share the same SemanticController object.  This means that
    // things probably won't work correctly when analyzing a multi-threaded specimen, but it keeps this demo more
    // understandable.
    virtual Trigger *clone() /*override*/ { return this; }

    // Arm this callback so it starts monitoring instructions for the trigger address.
    void arm(rose_addr_t trigger_va) {
        this->trigger_va = trigger_va;
        armed = true;
    }

    // Called for every instruction by every thread. When the trigger address is reached for the first time, create a new
    // CloneDetector and throw it.  The main() will catch it and start its analysis.
    virtual bool operator()(bool enabled, const Args &args) /*override*/ {
        if (enabled && armed && !triggered && args.insn->get_address()==trigger_va) {
            triggered = true;
            throw new CloneDetector(args.thread, opt);
        }
        return enabled;
    }
};

/******************************************************************************************************************************/
int
main(int argc, char *argv[], char *envp[])
{
    std::ios::sync_with_stdio();

    // Parse command-line switches that we recognize.
    Switches opt;
    for (int i=1; i<argc && '-'==argv[i][0]; /*void*/) {
        bool consume = false;
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h") || !strcmp(argv[i], "-?")) {
            usage(argv[0], 0);
        } else if (!strncmp(argv[i], "--database=", 11)) {
            opt.dbname = argv[i]+11;
            consume = true;
        } else if (!strcmp(argv[i], "--verbose")) {
            opt.verbose = consume = true;
        } else if (!strncmp(argv[i], "--nfuzz=", 8)) {
            char *rest;
            opt.nfuzz = strtoul(argv[i]+8, &rest, NULL);
            if (','==*rest)
                opt.firstfuzz = strtoul(rest+1, NULL, 0);
            consume = true;
        } else if (!strncmp(argv[i], "--ninputs=", 10)) {
            char *rest;
            opt.npointers = opt.nnonpointers = strtoul(argv[i]+10, &rest, 0);
            if (','==*rest)
                opt.nnonpointers = strtoul(rest+1, NULL, 0);
            consume = true;
        } else if (!strncmp(argv[i], "--min-function-size=", 20)) {
            opt.min_funcsz = strtoul(argv[i]+20, NULL, 0);
            consume = true;
        } else if (!strncmp(argv[i], "--max-insns=", 12)) {
            opt.max_insns = strtoul(argv[i]+12, NULL, 0);
            consume = true;
        }
        
        if (consume) {
            memmove(argv+i, argv+i+1, (argc-i)*sizeof(argv[0])); // include terminating NULL at argv[argc]
            --argc;
        } else {
            ++i;
        }
    }

    // Our instruction callback.  We can't set its trigger address until after we load the specimen, but we want to register
    // the callback with the simulator before we create the first thread.
    Trigger clone_detection_trigger(opt);

    // All of this is standard boilerplate and documented in the first page of the simulator doxygen.
    RSIM_Linux32 sim;
    sim.install_callback(new RSIM_Tools::UnhandledInstruction); // needed by some versions of ld-linux.so
    sim.install_callback(&clone_detection_trigger);             // it mustn't be destroyed while the simulator's running
    int n = sim.configure(argc, argv, envp);
    sim.exec(argc-n, argv+n);

    // Now that we've loaded the specimen into memory and created its first thread, figure out its original entry point (OEP)
    // and arm the clone_detection_trigger so that it triggers clone detection when the OEP is reached.  This will allow the
    // specimen's dynamic linker to run.
    rose_addr_t trigger_va = sim.get_process()->get_ep_orig_va();
    clone_detection_trigger.arm(trigger_va);

    // Allow the specimen to run until it reaches the clone detection trigger point, at which time a CloneDetector object is
    // thrown for that thread that reaches it first.
    try {
        sim.get_process()->get_main_thread()->tracing(TRACE_MISC)
            ->mesg("running specimen until we hit the analysis trigger address: 0x%08"PRIx64, trigger_va);
        sim.main_loop();
    } catch (CloneDetector *clone_detector) {
        clone_detector->analyze();
    }

    return 0;
}

#else
int main(int, char *argv[])
{
#ifndef RSIM_CloneDetectionSemantics_H
    std::cerr <<argv[0] <<": this program is not configured properly.\n"
              <<"  You must apply the CloneDetection.patch to the simulator project\n"
              <<"  and then recompile the simulator in a clean build directory. I.e,\n"
              <<"  something along these lines:\n"
              <<"    $ (cd $ROSE_SRC/projects/simulator;\n"
              <<"       patch -p3 <clone_detection/CloneDetection.patch)\n"
              <<"    $ make clean\n"
              <<"    $ make CloneDetection\n";
#else
    std::cerr <<argv[0] <<": the simulator not supported on this platform" <<std::endl;
#endif
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
