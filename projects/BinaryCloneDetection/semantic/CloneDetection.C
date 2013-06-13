#include "rose.h"

#include "BinaryLoader.h"
#include "BinaryPointerDetection.h"
#include "YicesSolver.h"
#include "DwarfLineMapper.h"
#include "CloneDetectionLib.h"

// We'd like to not have to depend on a particular relational database management system, but ROSE doesn't have
// support for anything like ODBC.  The only other options are either to generate SQL output, which precludes being
// able to perform queries, or using sqlite3x.h, which is distributed as part of ROSE.  If you want to use a RDMS other
// than SQLite3, then dump SQL from the generated SQLite3 database and load it into whatever RDMS you want.
#include "sqlite3x.h"
using namespace sqlite3x; // its top-level class name start with "sqlite3_"

#ifdef ROSE_HAVE_GCRYPT_H
#include <gcrypt.h>
#endif

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>
#include <boost/algorithm/string/regex.hpp>

static std::string argv0; // base name of argv[0]

static void usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [FRONTEND_SWITCHES] SPECIMEN\n"
              <<"   --database=STR\n"
              <<"                     Specifies the name of the database into which results are placed.\n"
              <<"                     The default is \"clones.db\".\n"
              <<"   --follow-calls\n"
              <<"                     Normally, x86 CALL instructions are not followed, but rather consume\n"
              <<"                     the next input value as a return value.  This switch disables\n"
              <<"                     that special handling so that the CALL executes normally.  However,\n"
              <<"                     if the called address is invalid then the special handling still applies.\n"
              <<"   --function=ADDR\n"
              <<"                     Analyze only the specified function (provided it also satisfies\n"
              <<"                     all other selection criteria.  Normally all functions are considered.\n"
              <<"                     This switch may appear more than once to select multiple functions.\n"
              <<"   --init-memory\n"
              <<"                     If specified, most of memory is initialized according to a linear\n"
              <<"                     congruential generator seeded with one of the input values.  The\n"
              <<"                     default is that each read of an uninitialized memory address consumes\n"
              <<"                     the next input value from the input group.\n"
              <<"   --link\n"
              <<"                     Perform dynamic linking and analyze the libraries along with the\n"
              <<"                     specified executable. The default is to not link.\n"
              <<"   --min-function-size=N\n"
              <<"                     Minimum size of functions to analyze. Any function containing fewer\n"
              <<"                     than N instructions is not processed. The default is to analyze all\n"
              <<"                     functions.\n"
              <<"   --max-insns=N\n"
              <<"                     Maximum number of instructions to simulate per function. The default\n"
              <<"                     is 5000.\n"
              <<"   --nfuzz=N[,START]\n"
              <<"                     Number of times to fuzz test each function (default 10) and the\n"
              <<"                     sequence number of the first test (default 0).\n"
              <<"   --ninputs=N1[,N2]\n"
              <<"                     Number of input values to supply each time a function is run. When\n"
              <<"                     N1 and N2 are both specified, then N1 is the number of pointers and\n"
              <<"                     N2 is the number of non-pointers.  When only N1 is specified it will\n"
              <<"                     indicate the number of pointers and the number of non-pointers. The\n"
              <<"                     default is 20 pointers and 100 non-pointers.  If a function\n"
              <<"                     requires more input values, then null/zero are supplied to it.\n"
              <<"   --permute-inputs=N\n"
              <<"                     Normally, each input group contains random non-pointer values.  If\n"
              <<"                     this switch is given with N greater than 1 then the first N non-pointer\n"
              <<"                     values of an input group is a permutation of the first N non-pointer\n"
              <<"                     values of an earlier input group.  For instance, if N is three then\n"
              <<"                     input groups will be generated in groups of six (6=3!).  The N\n"
              <<"                     specified here may be larger than the N1 value for the --ninputs\n"
              <<"                     switch, in which case some of the values are zero.  It probably\n"
              <<"                     doesn't make much sense to use --permute-inputs with --init-memory.\n"
              <<"   --[no-]pointers\n"
              <<"                     Turn pointer analysis on or off (the default is off).  When pointer\n"
              <<"                     analysis is turned on, each function is analyzed to find memory addresses\n"
              <<"                     that are used as code or data pointers.  This information is used when\n"
              <<"                     deciding whether to consume a pointer or non-pointer input.  When the\n"
              <<"                     analysis is turned off, only non-pointer inputs are used.\n"
              <<"   --progress\n"
              <<"                     Force a progress bar to be displayed even if the standard error stream\n"
              <<"                     is not a terminal and even if the verbosity is more than silent.\n"
              <<"   --verbose\n"
              <<"   --verbosity=(silent|laconic|effusive)\n"
              <<"                     How much diagnostics to show.  The default is silent.  The \"--verbose\"\n"
              <<"                     switch does the same thing as \"--verbosity=effusive\".\n";
    exit(exit_status);
}

static void die(std::string mesg="")
{
    std::cerr <<argv0 <<": " <<mesg;
    if (mesg.empty()) {
        std::cerr <<"unspecified failure\n";
        abort(); // for post mortem
    } else if (mesg.find_last_of('\n')==std::string::npos) {
        std::cerr <<"\n";
    }
    std::cerr <<"  See \"--help\" for basic usage information.\n";
    exit(1);
}





/*****************************************************************************************************************************/








namespace CloneDetection {



/*******************************************************************************************************************************
 *                                      Clone Detection Analysis
 *******************************************************************************************************************************/


/** Main driving function for clone detection.  This is the class that chooses inputs, runs each function, and looks at the
 *  outputs to decide how to partition the functions.  It does this repeatedly in order to build a PartitionForest. The
 *  analyze() method is the main entry point. */
class Analysis {
protected:
    sqlite3_connection sqlite;          /**< Database in which to place results; may already exist */
    OutputGroups output_groups;         /**< Distinct groups of output values from fuzz tests. */
    Switches opt;                       /**< Analysis configuration switches from the command line. */
    FunctionIdMap func_ids;             /**< Mapping from SgAsmFunction* to the ID stored in the database. */

    Policy<State, PartialSymbolicSemantics::ValueType> policy;
    typedef X86InstructionSemantics<Policy<State, PartialSymbolicSemantics::ValueType>,
                                    PartialSymbolicSemantics::ValueType> Semantics;
    Semantics semantics;

public:
    Analysis(const Switches &opt): opt(opt), policy(opt), semantics(policy) {
        open_db(opt.dbname);
    }

    void open_db(const std::string &dbname) {
        sqlite.open(dbname.c_str());
        sqlite.busy_timeout(15*60*1000); // 15 minutes
    }

    // Get a list of functions to analyze.
    Functions find_functions(SgAsmInterpretation *interp, const std::set<rose_addr_t> &limited) {
        Functions retval;
        std::vector<SgAsmFunction*> allfuncs = SageInterface::querySubTree<SgAsmFunction>(interp);
        for (size_t i=0; i<allfuncs.size(); ++i) {
            size_t ninsns = SageInterface::querySubTree<SgAsmInstruction>(allfuncs[i]).size();
            if ((0==opt.min_funcsz || ninsns >= opt.min_funcsz) &&
                (limited.empty() || limited.find(allfuncs[i]->get_entry_va())!=limited.end()))
                retval.insert(allfuncs[i]);
        }
        return retval;
    }

    // Perform a pointer-detection analysis on the specified function. We'll need the results in order to determine whether a
    // function input should consume a pointer or a non-pointer from the input value set.
    PointerDetector* detect_pointers(SgAsmFunction *func) {
        if (!opt.pointer_analysis)
            return NULL;

        // Choose an SMT solver. This is completely optional.  Pointer detection still seems to work fairly well (and much,
        // much faster) without an SMT solver.
        SMTSolver *solver = NULL;
#if 0   // optional code
        if (YicesSolver::available_linkage())
            solver = new YicesSolver;
#endif
        InstructionProvidor insn_providor(func);
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection: " <<function_to_str(func) <<" pointer detection analysis\n";
        PointerDetector *pd = new PointerDetector(&insn_providor, solver);
        pd->initial_state().registers.gpr[x86_gpr_sp] = SymbolicSemantics::ValueType<32>(policy.INITIAL_STACK);
        pd->initial_state().registers.gpr[x86_gpr_bp] = SymbolicSemantics::ValueType<32>(policy.INITIAL_STACK);
        //pd.set_debug(stderr);
        try {
            pd->analyze(func);
        } catch (...) {
            // probably the instruction is not handled by the semantics used in the analysis.  For example, the
            // instruction might be a floating point instruction that isn't handled yet.
            std::cerr <<"CloneDetection: pointer analysis FAILED for " <<function_to_str(func) <<"\n";
        }
        if (opt.verbosity>=EFFUSIVE) {
            const PointerDetector::Pointers plist = pd->get_pointers();
            for (PointerDetector::Pointers::const_iterator pi=plist.begin(); pi!=plist.end(); ++pi) {
                std::cerr <<"    ";
                if (pi->type & BinaryAnalysis::PointerAnalysis::DATA_PTR)
                    std::cerr <<"data ";
                if (pi->type & BinaryAnalysis::PointerAnalysis::CODE_PTR)
                    std::cerr <<"code ";
                std::cerr <<"pointer at " <<pi->address <<"\n";
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

    static std::string
    digest_to_str(const unsigned char digest[20]) {
        std::string digest_str;
        for (size_t i=20; i>0; --i) {
            digest_str += "0123456789abcdef"[(digest[i-1] >> 4) & 0xf];
            digest_str += "0123456789abcdef"[digest[i-1] & 0xf];
        }
        return digest_str;
    }

    struct FuncStats {
        FuncStats(): file_id(-1), isize(0), dsize(0), size(0), ninsns(0) {}
        FuncStats(size_t isize, size_t dsize, size_t size, size_t ninsns, const std::string &digest)
            : file_id(-1), isize(isize), dsize(dsize), size(size), ninsns(ninsns), digest(digest) {}
        int file_id;
        size_t isize, dsize, size, ninsns;
        std::string digest;                             // SHA1 hash of function bytes in virtual address order if known
    };


    // Save each function to the database. Updates the mapping from function object to ID number.
    // We have to do this in a database-efficient manner without holding a transaction lock too long because we're
    // likely to be running in parallel.
    void save_functions(SgAsmInterpretation *interp, const Functions &functions) {
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection: saving function information and assembly listings...\n";

        typedef std::map<rose_addr_t/*entry_va*/, SgAsmFunction*> AddrFunc;
        typedef std::map<std::string/* sha1 */, SgAsmFunction*> Sha1Func;
        typedef std::map<SgAsmFunction*, FuncStats> Stats;

        struct InstructionSelector: SgAsmFunction::NodeSelector {
            virtual bool operator()(SgNode *node) { return isSgAsmInstruction(node)!=NULL; }
        } iselector;

        struct DataSelector: SgAsmFunction::NodeSelector {
            virtual bool operator()(SgNode *node) { return isSgAsmStaticData(node)!=NULL; }
        } dselector;

        // Pre-compute some function info that doesn't depend on the database.
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   calculating function information from AST"
                      <<" (" <<functions.size() <<" function" <<(1==functions.size()?"":"s") <<")\n";
        AddrFunc addrfunc;
        Sha1Func sha1func;
        Stats stats;
        assert(gcry_md_get_algo_dlen(GCRY_MD_SHA1)==20);
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            SgAsmFunction *func = *fi;
            ExtentMap e_insns, e_data, e_total;
            size_t ninsns = func->get_extent(&e_insns, NULL, NULL, &iselector);
            func->get_extent(&e_data,  NULL, NULL, &dselector);
            func->get_extent(&e_total);
            uint8_t digest[20];
            func->get_sha1(digest);
            std::string digest_str = digest_to_str(digest);
            stats[func] = FuncStats(e_insns.size(), e_data.size(), e_total.size(), ninsns, digest_str);
            addrfunc[func->get_entry_va()] = func;
            sha1func[digest_str] = func;
        }

        // Populate the semantic_files table (assuming our functions can come from multiple files)
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   populating file table from binary function info\n";
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi)
            stats[*fi].file_id = get_file_id(filename_for_function(*fi));
        lock.commit();

        // Scan the whole function table and keep only those entries that correspond to our functions.  Since our functions
        // might come from multiple files, there isn't an easy way to limit the query to only those files (we could use "in"
        // but this seems about as fast).
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   getting function IDs from the database\n";
        sqlite3_command cmd1(sqlite, "select id, entry_va, file_id, digest from semantic_functions");
        lock.begin(sqlite3_transaction::LOCK_IMMEDIATE);
        sqlite3_reader c1 = cmd1.executereader();
        while (c1.read()) {
            int function_id = c1.getint(0);
            rose_addr_t entry_va = c1.getint64(1);
            int file_id = c1.getint(2);
            std::string digest = c1.getstring(3);
            Sha1Func::iterator sha1_found = sha1func.find(digest);
            SgAsmFunction *func = NULL;
            if (sha1_found!=sha1func.end()) {
                func = sha1_found->second;
            } else {
                AddrFunc::iterator addr_found = addrfunc.find(entry_va);
                if (addr_found!=addrfunc.end()) {
                    func = addr_found->second;
                }
            }
            // Functions are the same if they have either the same address or message digest, and they come from the same file.
            if (func!=NULL && file_id==stats[func].file_id)
                func_ids[func] = function_id;
        }
        lock.commit();
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   " <<func_ids.size() <<" of our functions " <<(1==func_ids.size()?"is":"are")
                      <<" already in the database\n";

        // Figure out which functions are not in the database.  By time we get done with this loop some other process might
        // have added those functions, but we'll handle that later.
        Functions to_add;
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            SgAsmFunction *func = *fi;
            if (func_ids.find(func)==func_ids.end())
                to_add.insert(func);
        }

        // Add necessary functions to the semantic_functions table, but be careful because some other process might have added
        // them since we released the lock above.
        if (opt.verbosity>=LACONIC) {
            std::cerr <<"CloneDetection:   adding " <<to_add.size() <<" function ID" <<(1==to_add.size()?"":"s")
                      <<" to the database\n";
        }
        sqlite3_command cmd2(sqlite, "select coalesce(max(id),-1) from semantic_functions where entry_va=? and file_id=?");
        sqlite3_command cmd3(sqlite, "insert into semantic_functions"
                             // 1   2         3         4        5      6      7     8       9
                             " (id, entry_va, funcname, file_id, isize, dsize, size, ninsns, digest)"
                             " values (?,?,?,?,?,?,?,?,?)");
        lock.begin(sqlite3_transaction::LOCK_IMMEDIATE);
        int next_id = sqlite.executeint("select coalesce(max(id),-1)+1 from semantic_functions");
        Functions added;
        for (Functions::iterator fi=to_add.begin(); fi!=to_add.end(); ++fi) {
            SgAsmFunction *func = *fi;
            cmd2.bind(1, func->get_entry_va());
            cmd2.bind(2, stats[func].file_id);
            int func_id = cmd2.executeint();
            if (func_id<0) {
                func_ids[func] = next_id;
                cmd3.bind(1, next_id);
                cmd3.bind(2, func->get_entry_va());
                cmd3.bind(3, func->get_name());
                cmd3.bind(4, stats[func].file_id);
                const FuncStats &s = stats[func];
                cmd3.bind(5, s.isize);
                cmd3.bind(6, s.dsize);
                cmd3.bind(7, s.size);
                cmd3.bind(8, s.ninsns);
                cmd3.bind(9, s.digest);
                cmd3.executenonquery();
                added.insert(func);
                ++next_id;
            }
        }
        lock.commit();
        if (opt.verbosity>=LACONIC) {
            std::cerr <<"CloneDetection:   added " <<added.size() <<" function ID" <<(1==added.size()?"":"s")
                      <<" to the database\n";
        }

        // For each function we added to the database, also add the assembly listing. We assume that if the function already
        // exists in the database then its assembly listing also exists, but this might not be true if the process is
        // interruped here.
        sqlite3_command cmd4(sqlite,
                             "insert into semantic_instructions"
                             // 1        2     3         4            5         6            7
                             " (address, size, assembly, function_id, position, src_file_id, src_line)"
                             " values (?,?,?,?,?,?,?)");
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   computing address-to-source mapping\n";
        SgBinaryComposite *binfile = SageInterface::getEnclosingNode<SgBinaryComposite>(interp);
        assert(binfile!=NULL);
        BinaryAnalysis::DwarfLineMapper dlm(binfile);
        dlm.fix_holes();
        AsmUnparser unparser;
        //unparser.staticDataDisassembler.init(thread->get_process()->get_disassembler()); //FIXME
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   saving assembly code for each function\n";
        for (Functions::iterator ai=added.begin(); ai!=added.end(); ++ai) {
            SgAsmFunction *func = *ai;
            int func_id = func_ids[func];
            if (opt.verbosity>=EFFUSIVE)
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
                if (opt.verbosity>=EFFUSIVE)
                    std::cerr <<'.';
            }
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"\n";
        }
    }

    // Rewrite a call graph by removing dynamic linked function thunks.  If we were using ROSE's experimental Graph2 stuff
    // instead of the Boost Graph Library's (BGL) adjacency_list, we could modify the graph in place efficiently.  But with
    // adjacency_list it's more efficient to create a whole new graph. [Robb P. Matzke 2013-05-16]
    CG rewrite_cg(CG &src) {
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   rewriting call graph to eliminate dynamic-linking thunks\n";
        CG dst;
        typedef std::map<CG_Vertex, CG_Vertex> VertexMap;
        VertexMap vmap; // mapping from src to dst vertex
        boost::graph_traits<CG>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end)=vertices(src); vi!=vi_end; ++vi) {
            CG_Vertex f1 = *vi;
            SgAsmFunction *func1 = get(boost::vertex_name, src, f1);
            if (vmap.find(f1)==vmap.end() && 0!=(SgAsmFunction::FUNC_IMPORT & func1->get_reason()) && 1==out_degree(f1, src)) {
                CG_Vertex f2 = target(*(out_edges(*vi, src).first), src); // the function that f1 calls
                SgAsmFunction *func2 = get(boost::vertex_name, src, f2);
                // We found thunk F1 that calls function F2. We want to remove F1 from the returned graph and replace all edges
                // (X,F1) with (X,F2). Therefore, create two mappings in the vmap: F2->F2' and F1->F2'. Be careful because we
                // might have already added vertex F2' to the returned graph.
                VertexMap::iterator f2i = vmap.find(f2);
                CG_Vertex f2prime;
                if (f2i==vmap.end()) {
                    f2prime = add_vertex(dst);
                    put(boost::vertex_name, dst, f2prime, func2);
                    vmap[f2] = f2prime;
                } else {
                    f2prime = f2i->second;
                }
                vmap[f1] = f2prime;
                if (opt.verbosity>=EFFUSIVE) {
                    FunctionIdMap::const_iterator id1=func_ids.find(func1), id2=func_ids.find(func2);
                    std::cerr <<"CloneDetection:     thunk " <<function_to_str(func1)
                              <<" delegated to " <<function_to_str(func2) <<"\n";
                }
            } else {
                CG_Vertex f1prime = add_vertex(dst);
                vmap[f1] = f1prime;
                put(boost::vertex_name, dst, f1prime, func1);
            }
        }

        // Now add the edges
        boost::graph_traits<CG>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end)=edges(src); ei!=ei_end; ++ei) {
            CG_Vertex f1 = source(*ei, src);
            CG_Vertex f2 = target(*ei, src);
            VertexMap::iterator f1i = vmap.find(f1);
            VertexMap::iterator f2i = vmap.find(f2);
            assert(f1i!=vmap.end());
            assert(f2i!=vmap.end());
            CG_Vertex f1prime = f1i->second;
            CG_Vertex f2prime = f2i->second;
            add_edge(f1prime, f2prime, dst);
        }
        return dst;
    }
    
    // Save the function call graph.
    void save_cg(SgAsmInterpretation *interp) {
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection: saving call graph...\n";

        // Generate the full call graph, then rewrite the call graph so calls to thunks for dynamically-linked functions look
        // like they're calls directly to the dynamically linked function.
        CG cg1 = BinaryAnalysis::FunctionCall().build_cg_from_ast<CG>(interp);
        CG cg2 = rewrite_cg(cg1);

        // Filter out vertices (and their incident edges) if the vertex is a function which is not part of the database (has no
        // function ID).
        struct CGFilter: BinaryAnalysis::FunctionCall::VertexFilter {
            const FunctionIdMap &func_ids;
            CGFilter(const FunctionIdMap &func_ids): func_ids(func_ids) {}
            virtual bool operator()(BinaryAnalysis::FunctionCall*, SgAsmFunction *vertex) /*override*/ {
                return func_ids.find(vertex)!=func_ids.end();
            }
        } vertex_filter(func_ids);
        BinaryAnalysis::FunctionCall copier;
        copier.set_vertex_filter(&vertex_filter);
        CG cg = copier.copy<CG>(cg2);

        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);

        // Delete from the database those call graph edges which have a source or target vertex that is one of the functions
        // we're analyzing.  This would be much easier if we could guarantee that all the functions we're analyzing come from
        // the same file.
        sqlite.executenonquery("drop table if exists tmp_cg");
        sqlite.executenonquery("create temporary table tmp_cg (func_id integer)");
        sqlite3_command cmd1(sqlite, "insert into tmp_cg (func_id) values (?)");
        for (FunctionIdMap::const_iterator fi=func_ids.begin(); fi!=func_ids.end(); ++fi) {
            cmd1.bind(1, fi->second);
            cmd1.executenonquery();
        }
        sqlite.executenonquery("delete from semantic_cg where exists"
                               " (select * from tmp_cg where caller = func_id or callee = func_id)");

        // Add the new call graph into to the database (that's still locked)
        sqlite3_command cmd2(sqlite, "insert into semantic_cg (caller, callee) values (?,?)");
        boost::graph_traits<CG>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end)=edges(cg); ei!=ei_end; ++ei) {
            CG_Vertex caller_v = source(*ei, cg);
            CG_Vertex callee_v = target(*ei, cg);
            SgAsmFunction *caller = get(boost::vertex_name, cg, caller_v);
            SgAsmFunction *callee = get(boost::vertex_name, cg, callee_v);
            int caller_id = func_ids.at(caller);
            int callee_id = func_ids.at(callee);
            cmd2.bind(1, caller_id);
            cmd2.bind(2, callee_id);
            cmd2.executenonquery();
        }

        lock.commit();
    };

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
        int status __attribute__((unused)) = fsetpos(f, &pos);
        assert(status>=0);
        for (size_t i=0; i<nread; ++i)
            if (!isascii(buf[i]))
                return false;
        return true;
    }

    // Suck source code into the database.  For any file that can be read and which does not have lines already saved
    // in the semantic_sources table, read each line of the file and store them in semantic_sources.
    void save_files() {
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection: saving source code listings for each function...\n";
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
                    if (opt.verbosity>=EFFUSIVE)
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
                        if (opt.verbosity>=EFFUSIVE)
                            std::cerr <<".";
                    }
                    if (line)
                        free(line);
                }
                if (opt.verbosity>=EFFUSIVE)
                    std::cerr <<"\n";
            }
            if (f)
                fclose(f);
        }
    }

    // Choose input values for fuzz testing.  The input values come from the database if they exist there, otherwise they are
    // chosen and written to the database. The set will consist of some number of non-pointers and pointers.  The pointers are
    // chosen to be randomly null or non-null, but the non-null values are not random.  Pointer values are only used if pointer
    // detection analysis is performed (i.e., the "--pointers" switch).  The non-pointers are chosen randomly or are a
    // permutation of a previous group's non-pointer values, depending on the "--permute-inputs" switch.
    InputGroup choose_inputs(size_t inputgroup_id) {
        InputGroup inputs;

        // Hold write lock while we check for input values and either read or create them. Otherwise some other
        // process might concurrently decide that input values don't exist and we'll have two processes trying to create
        // the input values that might differ.
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);

        sqlite3_command cmd1(sqlite, "select vtype, val from semantic_inputvalues where id = ? order by pos");
        cmd1.bind(1, inputgroup_id);
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
            size_t np = factorial(opt.permute_inputs); // number of possible permutations
            size_t pn = inputgroup_id % np; // non-pointer permutation number; zero means no permutation, but random values

            std::vector<uint64_t> nonpointers;
            if (0==pn) {
                static unsigned integer_modulus = 256;  // arbitrary;
                for (size_t i=0; i<opt.nnonpointers; ++i)
                    nonpointers.push_back(rand() % integer_modulus);
            } else {
                sqlite3_command cmd4(sqlite, "select val from semantic_inputvalues where id=? and vtype='N' order by pos");
                size_t base_group_id = (inputgroup_id / np) * np; // input group that serves as the base
                cmd4.bind(1, base_group_id);
                sqlite3_reader c4 = cmd4.executereader();
                while (c4.read())
                    nonpointers.push_back(c4.getint(0));
                if (opt.permute_inputs>nonpointers.size())
                    nonpointers.resize(opt.permute_inputs, 0);
                permute(nonpointers, pn, opt.permute_inputs);
            }
            for (size_t i=0; i<nonpointers.size(); ++i) {
                cmd3.bind(1, inputgroup_id);
                cmd3.bind(2, "N");
                cmd3.bind(3, i);
                cmd3.bind(4, nonpointers[i]);
                cmd3.executenonquery();
            }
                
            static unsigned nonnull_denom = 3;      // probability of a non-null pointer is 1/N
            for (size_t i=0; i<opt.npointers; ++i) {
                uint64_t val = rand()%nonnull_denom ? 0 : allocate_page();
                inputs.add_pointer(val);
                cmd3.bind(1, inputgroup_id);
                cmd3.bind(2, "P");
                cmd3.bind(3, i);
                cmd3.bind(4, val);
                cmd3.executenonquery();
            }
        }

        lock.commit();
        return inputs;
    }

    // Save output values into the database. Each fuzz test generates some number of output_values, all of which are collected
    // into a single container that we call an output group.  In order to cut down on the number of output groups in the
    // database, this function will reuse output groups that already exist.
    size_t save_outputs(OutputGroup &outputs, const VaId &func_va2id, const IdVa &func_id2va) {
        // We need a write lock for the duration, otherwise some other process might determine that an output set
        // doesn't exist and then try to create the same one we're about to create.
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);

        sqlite3_command cmd1(sqlite, "select coalesce(max(id),-1)+1 from semantic_outputvalues"); // next group ID
        sqlite3_command cmd3(sqlite, "insert into semantic_outputvalues (id, pos, val, vtype) values (?,?,?,?)");

        // Load output groups from the database if they've changed since last time we loaded.  If we're the only process
        // creating output groups then we'll only load the output groups when we first start.  We're assuming that no process
        // is deleting output groups and that groups are numbered consecutively starting at zero.
        int next_id = cmd1.executeint();
        if ((int)output_groups.size() != next_id) {
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: loading output groups from the database\n";
            load_output_groups(sqlite, &func_id2va, output_groups);
        }

        // Remove non-analyzed functions from the output callee list
        for (size_t i=0; i<outputs.callees_va.size(); ++i) {
            if (func_va2id.find(outputs.callees_va[i])==func_va2id.end())
                outputs.callees_va[i] = 0;
        }
        outputs.callees_va.erase(std::remove(outputs.callees_va.begin(), outputs.callees_va.end(), 0), outputs.callees_va.end());

        // Find an existing output group that matches the given values
        for (size_t i=0; i<output_groups.size(); ++i) {
            if (output_groups[i] == outputs)
                return i;
        }

        // Save this output group
        output_groups.resize(next_id+1);
        output_groups[next_id] = outputs;
        for (size_t i=0; i<outputs.values.size(); ++i) {
            cmd3.bind(1, next_id);
            cmd3.bind(2, i);
            cmd3.bind(3, outputs.values[i]);
            cmd3.bind(4, "V");
            cmd3.executenonquery();
        }
        for (size_t i=0; i<outputs.callees_va.size(); ++i) {
            VaId::const_iterator found = func_va2id.find(outputs.callees_va[i]); // have entry_va, need to save function ID
            assert(found!=func_va2id.end());
            cmd3.bind(1, next_id);
            cmd3.bind(2, i);
            cmd3.bind(3, found->second);
            cmd3.bind(4, "C");
            cmd3.executenonquery();
        }
        for (size_t i=0; i<outputs.syscalls.size(); ++i) {
            cmd3.bind(1, next_id);
            cmd3.bind(2, i);
            cmd3.bind(3, outputs.syscalls[i]);
            cmd3.bind(4, "S");
            cmd3.executenonquery();
        }
        if (outputs.fault!=AnalysisFault::NONE) {
            cmd3.bind(1, next_id);
            cmd3.bind(2, 0);
            cmd3.bind(3, outputs.fault);
            cmd3.bind(4, "F");
            cmd3.executenonquery();
        }

        lock.commit();
        return next_id;
    }

    // Return the name of a file containing the specified function.
    std::string filename_for_function(SgAsmFunction *function, bool basename=true) {
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

    // Analyze a single function by running it with the specified inputs and collecting its outputs. */
    OutputGroup fuzz_test(SgAsmInterpretation *interp, SgAsmFunction *function, InputGroup &inputs,
                          const InstructionProvidor &insns, const PointerDetector *pointers/*=NULL*/) {
        AnalysisFault::Fault fault = AnalysisFault::NONE;
        policy.reset(interp, function, &inputs, &insns, pointers);
        try {
            while (1) {
                if (!policy.state.registers.ip.is_known()) {
                    if (opt.verbosity>=EFFUSIVE)
                        std::cerr <<"CloneDetection: EIP value is not concrete\n";
                    fault = AnalysisFault::SEMANTICS;
                    break;
                }

                rose_addr_t insn_va = policy.state.registers.ip.known_value();
                if (policy.FUNC_RET_ADDR==insn_va) {
                    if (opt.verbosity>=EFFUSIVE)
                        std::cerr <<"CloneDetection: function returned\n";
                    fault = AnalysisFault::NONE;
                    break;
                }
                
                if (SgAsmx86Instruction *insn = isSgAsmx86Instruction(insns.get_instruction(insn_va))) {
                    semantics.processInstruction(insn);
                } else {
                    if (opt.verbosity>=EFFUSIVE)
                        std::cerr <<"CloneDetection: no instruction at " <<StringUtility::addrToString(insn_va) <<"\n";
                    fault = AnalysisFault::DISASSEMBLY;
                    break;
                }
            }
        } catch (const FaultException &e) {
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: analysis terminated by " <<AnalysisFault::fault_name(e.fault) <<"\n";
            fault = e.fault;
        } catch (const Exception &e) {
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: analysis terminated by semantic exception: " <<e.mesg <<"\n";
            fault = AnalysisFault::SEMANTICS;
        } catch (const BaseSemantics::Policy::Exception &e) {
            // Some exception in the policy, such as division by zero.
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: analysis terminated by FAULT_SEMANTICS: " <<e.mesg <<"\n";
            fault = AnalysisFault::SEMANTICS;
        } catch (const Semantics::Exception &e) { // X86InstructionSemantics<...>::Exception
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: analysis terminated by X86InstructionSemantics exception: " <<e.mesg <<"\n";
            fault = AnalysisFault::SEMANTICS;
        } catch (const SMTSolver::Exception &e) {
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: analysis terminated by SMT solver exception: " <<e.mesg <<"\n";
            fault = AnalysisFault::SMTSOLVER;
        }
        
        // Gather the function's outputs before restoring machine state.
        OutputGroup outputs = policy.get_outputs();
        outputs.fault = fault;
        return outputs;
    }

    // Detect functions that are semantically similar by running multiple iterations of partition_functions().
    void analyze(SgAsmInterpretation *interp) {
        std::cerr <<"CloneDetection: semantic analysis is starting\n";
        Functions functions = find_functions(interp, opt.functions);
        save_functions(interp, functions); // must be first because it initializes our func_ids data member
        save_files();
        save_cg(interp);
        InstructionProvidor insns(interp);

        // Mapping from function ID to function entry va and vice versa
        IdVa func_id2va;
        VaId func_va2id;
        for (FunctionIdMap::const_iterator fi=func_ids.begin(); fi!=func_ids.end(); ++fi) {
            func_id2va[fi->second] = fi->first->get_entry_va();
            func_va2id[fi->first->get_entry_va()] = fi->second;
        }

        typedef std::map<SgAsmFunction*, PointerDetector*> PointerDetectors;
        PointerDetectors pointers;
        sqlite3_command cmd1(sqlite, "insert into semantic_fio "
                             //1        2              3                  4                     5
                             "(func_id, inputgroup_id, pointers_consumed, nonpointers_consumed, instructions_executed,"
                             //6                   7                      8             9
                             " actual_outputgroup, effective_outputgroup, elapsed_time, cpu_time)"
                             " values (?,?,?,?,?,?,?,?,?)");
        sqlite3_command cmd2(sqlite, "select count(*) from semantic_fio where func_id=? and inputgroup_id=? limit 1");
        Progress progress(opt.nfuzz * functions.size());
        progress.force_output(opt.show_progress);
        for (size_t fuzz_number=opt.firstfuzz; fuzz_number<opt.firstfuzz+opt.nfuzz; ++fuzz_number) {
            InputGroup inputs = choose_inputs(fuzz_number);
            if (opt.verbosity>=LACONIC) {
                std::cerr <<"CloneDetection: " <<std::string(80, '#') <<"\n"
                          <<"CloneDetection: fuzz testing " <<functions.size() <<" function" <<(1==functions.size()?"":"s")
                          <<" with inputgroup " <<fuzz_number <<"\n"
                          <<"CloneDetection: using these input values:\n" <<inputs.toString();
            }
            for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
                if (opt.show_progress || opt.verbosity==SILENT)
                    progress.show();
                SgAsmFunction *func = *fi;
                inputs.reset();

                // Did we already test this function in a previous incarnation?
                cmd2.bind(1, func_ids[func]);
                cmd2.bind(2, fuzz_number);
                if (cmd2.executeint()>0) {
                    if (opt.verbosity>=LACONIC)
                        std::cerr <<"CloneDetection: " <<function_to_str(func) <<" fuzz test #" <<fuzz_number <<" SKIPPED\n";
                    continue;
                }

                // Get the results of pointer analysis.  We could have done this before any fuzz testing started, but by doing
                // it here we only need to do it for functions that are actually tested.
                PointerDetectors::iterator ip = pointers.find(func);
                if (ip==pointers.end())
                    ip = pointers.insert(std::make_pair(func, detect_pointers(func))).first;
                assert(ip!=pointers.end());

                // Run the test
                timeval start_time, stop_time;
                clock_t start_ticks = clock();
                gettimeofday(&start_time, NULL);
                if (opt.verbosity>=LACONIC)
                    std::cerr <<"CloneDetection: " <<function_to_str(func) <<" fuzz test #" <<fuzz_number <<"\n";
                OutputGroup outputs = fuzz_test(interp, func, inputs, insns, ip->second);
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
                if (opt.verbosity>=EFFUSIVE) {
                    std::cerr <<"CloneDetection: " <<function_to_str(func) <<" fuzz test #" <<fuzz_number <<" output values:\n";
                    outputs.print(std::cerr, "", "CloneDetection:  ");
                }
                size_t outputgroup_id = save_outputs(outputs, func_va2id, func_id2va);
                sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE, sqlite3_transaction::DEST_COMMIT);
                if (cmd2.executeint()>0) {
                    std::cerr <<"CloneDetection: " <<function_to_str(func) <<" fuzz test #" <<fuzz_number <<" ALREADY PRESENT\n";
                } else {
                    cmd1.bind(1, func_ids[func]);
                    cmd1.bind(2, fuzz_number);
                    cmd1.bind(3, inputs.pointers_consumed());
                    cmd1.bind(4, inputs.integers_consumed());
                    cmd1.bind(5, policy.ninsns);
                    cmd1.bind(6, outputgroup_id);
                    cmd1.bind(7, outputgroup_id);
                    cmd1.bind(8, elapsed_time);
                    cmd1.bind(9, cpu_time);
                    cmd1.executenonquery();
                }
            }
        }
        progress.clear();
        std::cerr <<"CloneDetection: final results stored in " <<opt.dbname <<"\n";
    }
};

} // namespace

/******************************************************************************************************************************/
int
main(int argc, char *argv[], char *envp[])
{
    using namespace CloneDetection;

    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

#ifdef ROSE_HAVE_GCRYPT_H
    if (!gcry_check_version(GCRYPT_VERSION)) {
        std::cerr <<argv0 <<": libgcrypt version mismatch\n";
        exit(1);
    }
#endif
    
    // Parse command-line switches that we recognize.
    Switches opt;
    for (int i=1; i<argc && '-'==argv[i][0]; /*void*/) {
        bool consume = false;
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h") || !strcmp(argv[i], "-?")) {
            usage(0);
        } else if (!strncmp(argv[i], "--database=", 11)) {
            opt.dbname = argv[i]+11;
            consume = true;
        } else if (!strcmp(argv[i], "--follow-calls")) {
            opt.follow_calls = consume = true;
        } else if (!strncmp(argv[i], "--function=", 11)) {
            char *rest;
            rose_addr_t va = strtoull(argv[i]+11, &rest, 0);
            if (rest==argv[i]+11 || *rest) {
                std::cerr <<argv0 <<": --function should specify a function entry address\n";
                exit(1);
            }
            opt.functions.insert(va);
        } else if (!strcmp(argv[i], "--link")) {
            opt.link = consume = true;
        } else if (!strncmp(argv[i], "--min-function-size=", 20)) {
            opt.min_funcsz = strtoul(argv[i]+20, NULL, 0);
            consume = true;
        } else if (!strncmp(argv[i], "--max-insns=", 12)) {
            opt.max_insns = strtoul(argv[i]+12, NULL, 0);
            consume = true;
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
        } else if (!strncmp(argv[i], "--permute-inputs=", 17)) {
            opt.permute_inputs = strtoul(argv[i]+17, NULL, 0);
            consume = true;
        } else if (!strcmp(argv[i], "--pointers")) {
            opt.pointer_analysis = true;
            consume = true;
        } else if (!strcmp(argv[i], "--no-pointers")) {
            opt.pointer_analysis = false;
            consume = true;
        } else if (!strcmp(argv[i], "--progress")) {
            opt.show_progress = true;
            consume = true;
        } else if (!strcmp(argv[i], "--verbose")) {
            opt.verbosity = EFFUSIVE;
            consume = true;
        } else if (!strncmp(argv[i], "--verbosity=", 12)) {
            size_t n = strtoul(argv[i]+12, NULL, 0);
            if (n>=EFFUSIVE || !strcmp(argv[i]+12, "effusive")) {
                opt.verbosity = EFFUSIVE;
            } else if (n>=LACONIC || !strcmp(argv[i]+12, "laconic")) {
                opt.verbosity = LACONIC;
            } else {
                opt.verbosity = SILENT;
            }
            consume = true;
        }
        
        if (consume) {
            memmove(argv+i, argv+i+1, (argc-i)*sizeof(argv[0])); // include terminating NULL at argv[argc]
            --argc;
        } else {
            ++i;
        }
    }

    opt.print(std::cerr, "CloneDetection: ");

    // Parse the binary container (ELF, PE, etc) but do not disassemble yet.
    std::cerr <<"CloneDetection: Parsing binary specimen...\n"
              <<"CloneDetection:   parsing container\n";
    int argc2 = argc+1;
    char **argv2 = new char*[argc+2];
    argv2[0] = argv[0];
    argv2[1] = strdup("-rose:read_executable_file_format_only");
    for (int i=1; i<argc; ++i)
        argv2[i+1] = argv[i];
    argv[argc2] = NULL;
    SgProject *project = frontend(argc2, argv2);

    // Find the primary interpretation (e.g., the PE, not DOS, interpretation in PE files).
    std::cerr <<"CloneDetection:   finding primary interpretation\n";
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    if (interps.empty())
        die("no binary specimen given");
    SgAsmInterpretation *interp = interps.back();

    // Get the shared libraries, map them, and apply relocation fixups. We have to do the mapping step even if we're not
    // linking with shared libraries, because that's what gets the various file sections lined up in memory for the
    // disassembler.
    if (opt.link)
        std::cerr <<"CloneDetection:   loading shared libraries\n";
    if (BinaryLoader *loader = BinaryLoader::lookup(interp)) {
        try {
            loader = loader->clone(); // so our settings are private
            if (opt.link) {
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
                std::cerr <<argv0 <<":     warning: " <<fixup_errors.size()
                          <<" relocation fixup error" <<(1==fixup_errors.size()?"":"s") <<" encountered\n";
            }
            if (SageInterface::querySubTree<SgAsmInterpretation>(project).size() != interps.size())
                std::cerr <<argv0 <<": warning: new interpretations created by the linker; mixed 32- and 64-bit libraries?\n";
        } catch (const BinaryLoader::Exception &e) {
            std::cerr <<argv0 <<": BinaryLoader error: " <<e.mesg <<"\n";
            exit(1);
        }
    } else {
        die("no suitable loader/linker found");
    }

    // Figure out what to disassemble.  If we did dynamic linking then we can mark the .got and .got.plt sections as read-only
    // because we've already filled them in with the addresses of the dynamically linked entities.  This will allow the
    // disassembler to know the successors for the indirect JMP instruction in the .plt section (the dynamic function thunks).
    assert(interp->get_map()!=NULL);
    MemoryMap map = *interp->get_map();
    if (opt.link) {
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
    std::cerr <<"CloneDetection:   disassembling and partitioning\n";
    if (Disassembler *disassembler = Disassembler::lookup(interp)) {
        disassembler = disassembler->clone(); // so our settings are private
#if 1 // FIXME [Robb P. Matzke 2013-05-14]
        // We need to handle -rose:disassembler_search, -rose:partitioner_search, and -rose:partitioner_config
        // command-line switches.
#endif
        if (opt.verbosity >= EFFUSIVE) {
            std::cerr <<"CloneDetection:     memory map for disassembly:\n";
            map.print(std::cerr, "CloneDetection:       ");
        }
        Partitioner *partitioner = new Partitioner();
        SgAsmBlock *gblk = partitioner->partition(interp, disassembler, &map);
        interp->set_global_block(gblk);
        gblk->set_parent(interp);
    } else {
        die("unable to disassemble this specimen");
    }

    // Save listings and dumps to aid debugging
    std::cerr <<"CloneDetection: saving dumps and listings to text files\n";
    backend(project);

    // Run the clone detection analysis
    CloneDetection::Analysis analysis(opt);
    analysis.analyze(interp);
    return 0;
}
