#include "rose.h"
#include "RSIM_Private.h"

// This source file can only be compiled if the simulator is enabled. Furthermore, it only makes sense to compile this file if
// the simulator is using our code from CloneDetectionSemantics.h (i.e., the CloneDetection.patch file has been applied to
// RSIM).
#if defined(ROSE_ENABLE_SIMULATOR) && defined(RSIM_CloneDetectionSemantics_H)

#include "RSIM_Linux32.h"
#include "BinaryPointerDetection.h"
#include "YicesSolver.h"

// We'd like to not have to depend on a particular relational database management system, but ROSE doesn't have
// support for anything like ODBC.  The only other options are either to generate SQL output, which precludes being
// able to perform queries, or using sqlite3x.h, which is distributed as part of ROSE.  If you want to use a RDMS other
// than SQLite3, then dump SQL from the generated SQLite3 database and load it into whatever RDMS you want.
#include "sqlite3x.h"
using namespace sqlite3x; // its top-level class name start with "sqlite3_"

typedef std::set<SgAsmFunction*> Functions;
typedef std::map<SgAsmFunction*, int> FunctionIdMap;
typedef CloneDetection::InputValues InputValues;
typedef std::set<uint32_t> OutputValues;

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
    enum { MAX_ITERATIONS = 10 };       /**< Maximum number of times we run the functions; max number of input sets. */
    enum { MAX_SIMSET_SIZE = 3 };       /**< Any similarity set containing more than this many functions will be partitioned. */

public:
    CloneDetector(RSIM_Thread *thread): thread(thread) {
        open_db("clones.db");
    }

    void open_db(const std::string &dbname) {
        if (-1==access(dbname.c_str(), F_OK)) {
            // SQLite3 database does not exist; load the schema from a file, which has been incorporated into this binary
            extern const char *clone_detection_schema; // see CloneDetectionSchema.C in the build directory
            FILE *f = popen(("sqlite3 " + dbname).c_str(), "w");
            assert(f!=NULL);
            fputs(clone_detection_schema, f);
            if (pclose(f)!=0) {
                std::cerr <<"sqlite3 command failed to initialize database \"" <<dbname <<"\"\n";
                return;
            }
        }
        sqlite.open(dbname.c_str());
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
        m->mesg("%s triggered; disassembling entire specimen image...\n", name);
        MemoryMap *map = disassembly_map(proc);
        std::ostringstream ss;
        map->dump(ss, "  ");
        m->mesg("%s: using this memory map for disassembly:\n%s", name, ss.str().c_str());
        SgAsmBlock *gblk = proc->disassemble(false/*take no shortcuts*/, map);
        delete map; map=NULL;
        std::vector<SgAsmFunction*> functions = SageInterface::querySubTree<SgAsmFunction>(gblk);
#if 0 /*DEBUGGING [Robb P. Matzke 2013-02-12]*/
        // Prune the function list to contain only what we want.
        for (std::vector<SgAsmFunction*>::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            if ((*fi)->get_name().compare("_Z1fRi")!=0)
                *fi = NULL;
        }
        functions.erase(std::remove(functions.begin(), functions.end(), (SgAsmFunction*)NULL), functions.end());
#endif
        return Functions(functions.begin(), functions.end());
    }

    // Perform a pointer-detection analysis on each function. We'll need the results in order to determine whether a function
    // input should consume a pointer or a non-pointer from the input value set.
    typedef std::map<SgAsmFunction*, CloneDetection::PointerDetector> PointerDetectors;
    PointerDetectors detect_pointers(RTS_Message *m, RSIM_Thread *thread, const Functions &functions) {
        // Choose an SMT solver. This is completely optional.  Pointer detection still seems to work fairly well (and much,
        // much faster) without an SMT solver.
        SMTSolver *solver = NULL;
#if 0   // optional code
        if (YicesSolver::available_linkage())
            solver = new YicesSolver;
#endif
        PointerDetectors retval;
        CloneDetection::InstructionProvidor *insn_providor = new CloneDetection::InstructionProvidor(thread->get_process());
        for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            m->mesg("%s: performing pointer detection analysis for \"%s\" at 0x%08"PRIx64" in %s",
                    name, (*fi)->get_name().c_str(), (*fi)->get_entry_va(), filename_for_function(*fi).c_str());
            CloneDetection::PointerDetector pd(insn_providor, solver);
            pd.initial_state().registers.gpr[x86_gpr_sp] = SYMBOLIC_VALUE<32>(thread->policy.INITIAL_STACK);
            pd.initial_state().registers.gpr[x86_gpr_bp] = SYMBOLIC_VALUE<32>(thread->policy.INITIAL_STACK);
            //pd.set_debug(stderr);
            pd.analyze(*fi);
            retval.insert(std::make_pair(*fi, pd));
#if 1 /*DEBUGGING [Robb P. Matzke 2013-01-24]*/
            if (m->get_file()) {
                const CloneDetection::PointerDetector::Pointers plist = pd.get_pointers();
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
#endif
        }
        return retval;
    }

    // Save each function to the database.  Returns a mapping from function object to ID number.
    FunctionIdMap save_functions(const Functions &functions) {
        FunctionIdMap retval;
        int func_id = sqlite.executeint("select coalesce(max(id),0)+1 from semantic_functions");
        sqlite3_command cmd1(sqlite, "insert into semantic_functions (id, entry_va, funcname, filename) values (?,?,?,?)");
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            SgAsmFunction *func = *fi;
            cmd1.bind(1, ++func_id);
            cmd1.bind(2, func->get_entry_va());
            cmd1.bind(3, func->get_name());
            cmd1.bind(4, filename_for_function(func));
            cmd1.executenonquery();
            retval.insert(std::make_pair(func, func_id));
        }
        return retval;
    }
    
    // Choose input values for fuzz testing.  The input values come from the database if they exist there, otherwise they are
    // chosen randomly and written to the database. The set will consist of some number of non-pointers and pointers. The
    // non-pointer values are chosen randomly, but limited to a certain range.  The pointers are chosen randomly to be null or
    // non-null and the non-null values each have one page allocated via simulated mmap() (i.e., the non-null values themselves
    // are not actually random).
    InputValues choose_inputs(size_t inputset_id) {
        static const size_t nintegers=3, npointers=3;
        CloneDetection::InputValues inputs;
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
            CloneDetection::InputValues inputs;
            for (size_t i=0; i<nintegers; ++i) {
                uint64_t val = rand() % integer_modulus;
                inputs.add_integer(val);
                cmd3.bind(1, inputset_id);
                cmd3.bind(2, "N");
                cmd3.bind(3, i);
                cmd3.bind(4, val);
                cmd3.executenonquery();
            }
            for (size_t i=0; i<npointers; ++i) {
                uint64_t val = rand()%nonnull_denom ? 0 : allocate_page();
                inputs.add_pointer(val);
                cmd3.bind(1, inputset_id);
                cmd3.bind(2, "P");
                cmd3.bind(3, i);
                cmd3.bind(4, val);
                cmd3.executenonquery();
            }
        }

        return inputs;
    }

    // Save output values into the database.  If an output set already exists in the database with these values, return that
    // set's ID instead of saving an new, identical set.
    size_t save_outputs(const OutputValues &outputs) {
        // Load output sets from the database
        if (output_sets.empty()) {
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
        size_t setid = output_sets.size();
        output_sets.resize(setid+1);
        output_sets[setid] = outputs;
        for (OutputValues::const_iterator oi=outputs.begin(); oi!=outputs.end(); ++oi) {
            sqlite3_command cmd2(sqlite, "insert into semantic_outputvalues (id, val) values (?,?)");
            cmd2.bind(1, setid);
            cmd2.bind(2, *oi);
            cmd2.executenonquery();
        }

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
    
    // Analyze a single function by running it with the specified inputs and collecting its outputs. */
    CloneDetection::Outputs<RSIM_SEMANTICS_VTYPE> *fuzz_test(SgAsmFunction *function, CloneDetection::InputValues &inputs,
                                                             const CloneDetection::PointerDetector &pointers) {
        RSIM_Process *proc = thread->get_process();
        RTS_Message *m = thread->tracing(TRACE_MISC);
        m->mesg("==========================================================================================");
        m->mesg("%s: fuzz testing function \"%s\" at 0x%08"PRIx64" in %s",
                name, function->get_name().c_str(), function->get_entry_va(), filename_for_function(function).c_str());

        // Not sure if saving/restoring memory state is necessary. I don't thing machine memory is adjusted by the semantic
        // policy's writeMemory() or readMemory() operations after the policy is triggered to enable our analysis.  But it
        // shouldn't hurt to save/restore anyway, and it's fast. [Robb Matzke 2013-01-14]
        proc->mem_transaction_start(name);
        pt_regs_32 saved_regs = thread->get_regs();

        // Trigger the analysis, resetting it to start executing the specified function using the input values and pointer
        // variable addresses we selected previously.
        thread->policy.trigger(function->get_entry_va(), &inputs, &pointers);

        // "Run" the function using our semantic policy.  The function will not "run" in the normal sense since: since our
        // policy has been triggered, memory access, function calls, system calls, etc. will all operate differently.  See
        // CloneDetectionSemantics.h and CloneDetectionTpl.h for details.
        try {
            thread->main();
        } catch (const Disassembler::Exception &e) {
            // Probably due to the analyzed function's RET instruction, but could be from other things as well. In any case, we
            // stop analyzing the function when this happens.
            m->mesg("%s: function disassembly failed at 0x%08"PRIx64": %s", name, e.ip, e.mesg.c_str());
        } catch (const CloneDetection::InsnLimitException &e) {
            // The analysis might be in an infinite loop, such as when analyzing "void f() { while(1); }"
            m->mesg("%s: %s", name, e.mesg.c_str());
        } catch (const RSIM_Semantics::InnerPolicy<>::Halt &e) {
            // The x86 HLT instruction appears in some functions (like _start) as a failsafe to terminate a process.  We need
            // to intercept it and terminate only the function analysis.
            m->mesg("%s: function executed HLT instruction at 0x%08"PRIx64, name, e.ip);
        } catch (const RSIM_SEMANTICS_POLICY::Exception &e) {
            // Some exception in the policy, such as division by zero.
            m->mesg("%s: %s", name, e.mesg.c_str());
        }
        
        // Gather the function's outputs before restoring machine state.
        bool verbose = true;
        CloneDetection::Outputs<RSIM_SEMANTICS_VTYPE> *outputs = thread->policy.get_outputs(verbose);
        thread->init_regs(saved_regs);
        proc->mem_transaction_rollback(name);
        return outputs;
    }

    // Detect functions that are semantically similar by running multiple iterations of partition_functions().
    void analyze() {
        thread->set_do_coredump(false);
        RTS_Message *m = thread->tracing(TRACE_MISC);
        thread->get_process()->mem_showmap(m, "Starting clone detection analysis", "  ");
        Functions functions = find_functions(m, thread->get_process());
        FunctionIdMap func_ids = save_functions(functions);

        PointerDetectors pointers = detect_pointers(m, thread, functions);

        sqlite3_command cmd1(sqlite, "insert into semantic_fio (func_id, inputset_id, outputset_id) values (?,?,?)");
        for (size_t i=0; i<MAX_ITERATIONS; ++i) {
            InputValues inputs = choose_inputs(i);
            m->mesg("####################################################################################################");
            m->mesg("%s: fuzz testing %zu function%s with inputset %zu", name, functions.size(), 1==functions.size()?"":"s", i);
            m->mesg("%s: using these input values:\n%s", name, inputs.toString().c_str());

            for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
                PointerDetectors::iterator ip = pointers.find(*fi);
                assert(ip!=pointers.end());
                CloneDetection::Outputs<RSIM_SEMANTICS_VTYPE> *outputs = fuzz_test(*fi, inputs, ip->second);
#if 1 /*DEBUGGING [Robb Matzke 2013-01-14]*/
                std::ostringstream output_values_str;
                OutputValues output_values = outputs->get_values();
                for (OutputValues::iterator ovi=output_values.begin(); ovi!=output_values.end(); ++ovi)
                    output_values_str <<" " <<*ovi;
                m->mesg("%s: function output values are {%s }", name, output_values_str.str().c_str());
#endif
                size_t outputset_id = save_outputs(outputs->get_values());
                cmd1.bind(1, func_ids[*fi]);
                cmd1.bind(2, i);
                cmd1.bind(3, outputset_id);
                cmd1.executenonquery();
            }
        }
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
public:
    Trigger(): trigger_va(0), armed(false), triggered(false) {}

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
            throw new CloneDetector(args.thread);
        }
        return enabled;
    }
};
            
    
/******************************************************************************************************************************/
int
main(int argc, char *argv[], char *envp[])
{
    std::ios::sync_with_stdio();

    // Our instruction callback.  We can't set its trigger address until after we load the specimen, but we want to register
    // the callback with the simulator before we create the first thread.
    Trigger clone_detection_trigger;

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
        sim.main_loop();
    } catch (CloneDetector *clone_detector) {
        clone_detector->analyze();
    }

    return 0;
}

#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
