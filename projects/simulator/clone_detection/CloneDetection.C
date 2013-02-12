#include "rose.h"
#include "RSIM_Private.h"

// This source file can only be compiled if the simulator is enabled. Furthermore, it only makes sense to compile this file if
// the simulator is using our code from CloneDetectionSemantics.h (i.e., the CloneDetection.patch file has been applied to
// RSIM).
#if defined(ROSE_ENABLE_SIMULATOR) && defined(RSIM_CloneDetectionSemantics_H)

#include "RSIM_Linux32.h"
#include "BinaryPointerDetection.h"
#include "YicesSolver.h"

/*******************************************************************************************************************************
 *                                      Partition Forest
 *******************************************************************************************************************************/

typedef std::set<SgAsmFunction*> Functions;
typedef CloneDetection::InputValues InputValues;
typedef std::set<uint32_t> OutputValues;

/** A forest that partitions functions into similarity classes. Each vertex contains a set of similar functions and the output
 *  values produced by that set of functions (they all produced the same output).  Each level of the tree provides a set of
 *  input values that were used when running the functions at that level of the tree.  Each vertex (except those at the top)
 *  has a pointer to a vertex at the next higher level of the tree; by following the upward pointers one can determine all the
 *  sets of input values that have been used to partition the whole set of functions. */
class PartitionForest {
public:
    /** Tree vertex.  A vertex contains a set of functions that are semantically similar because they all produced the same
     *  output for a given input. The output values are stored in the vertex while the input values are stored across the entire
     *  level of the tree. Each vertex not at the top of the tree has a pointer to its parent; the children of a parent
     *  partition the parent. */
    struct Vertex {
        Vertex *parent;                         // parent pointer
        Functions functions;                    // functions that are similar
        OutputValues outputs;                   // outputs for the level's given input
        std::set<Vertex*> children;             // all children of this vertex
        size_t id;                              // ID used when dumping to a DBMS
        Vertex(Vertex *parent, SgAsmFunction *func, const OutputValues &outputs): parent(parent), outputs(outputs), id(0) {
            functions.insert(func);
            if (parent) {
                assert(parent->functions.find(func)!=parent->functions.end());
                parent->children.insert(this);
            }
        }
        size_t get_level() const {
            size_t n = 0;
            for (Vertex *v=parent; v; v=v->parent) ++n;
            return n;
        }
        const Functions& get_functions() { return functions; }
    };
    typedef std::set<Vertex*> Vertices;

    /** One level of a tree.  A level is a list of sibling vertices that represent a partitioning of all tested functions into
     *  semantically similar groups.  Levels are numbered so that zero is the top level. A new level is created by partitioning
     *  each vertex from the next higher level according to a set of input values chosen for the new level.  If level L has V
     *  vertices, then level L+1 will have at least V vertices also. */
    struct Level {
        InputValues inputs;
        Vertices vertices;
        Level(const InputValues &inputs): inputs(inputs) {}
    };

    typedef std::vector<Level> Levels;
    Levels levels;

    /** Returns the number of levels in the partition forest.  Levels are numbered starting at zero. The return value is one
     * more than the maximum level that contains a vertex. */
    size_t nlevels() const { return levels.size(); }

    /** Create a new (empty) level.  This is the level into which the next higher level's vertices will be partitioned. The
     * return value is the new level number. */
    size_t new_level(const InputValues &inputs) {
        levels.push_back(Level(inputs));
        return levels.size()-1;
    }

    /** Return the list of vertices at a given level of the forest. */
    const Vertices& vertices_at_level(size_t level) const {
        assert(level<levels.size());
        return levels[level].vertices;
    }

    /** Returns the vertex from the specified set that contains the specified function. */
    Vertex* contains(const Vertices &vertices, SgAsmFunction *func) const {
        for (Vertices::const_iterator vi=vertices.begin(); vi!=vertices.end(); ++vi) {
            if ((*vi)->functions.find(func)!=(*vi)->functions.end())
                return *vi;
        }
        return NULL;
    }

    /** Returns the set of all leaf nodes in the forest. */
    Vertices get_leaves() const {
        Vertices retval;
        for (size_t i=0; i<levels.size(); ++i) {
            const Vertices &vertices = vertices_at_level(i);
            for (Vertices::const_iterator vi=vertices.begin(); vi!=vertices.end(); ++vi) {
                if ((*vi)->children.empty())
                    retval.insert(*vi);
            }
        }
        return retval;
    }
    
    /** Insert a function into a vertex of the forest, or create a new vertex. The (new) vertex into which the function was
     * inserted is either a child of @p parent or a root node. The @p outputs are used to select the vertex into which the
     * function is inserted (all functions of a particular vertex produced the same output when run with the same input). */
    void insert(SgAsmFunction *func, OutputValues outputs, PartitionForest::Vertex *parent) {
        Vertices candidates = parent ? parent->children : vertices_at_level(0);
        assert(!contains(candidates, func));
        for (Vertices::iterator vi=candidates.begin(); vi!=candidates.end(); ++vi) {
            Vertex *vertex = *vi;
            if (outputs.size()==vertex->outputs.size() && std::equal(outputs.begin(), outputs.end(), vertex->outputs.begin())) {
                vertex->functions.insert(func);
                return;
            }
        }
        size_t lno = parent ? parent->get_level() + 1 : 0;
        assert(lno<levels.size());
        levels[lno].vertices.insert(new Vertex(parent, func, outputs));
    }

    /** Print the entire forest for debugging output. */
    void print(std::ostream &o) const {
        for (size_t i=0; i<levels.size(); ++i) {
            if (levels[i].vertices.empty()) {
                o <<"partition forest level " <<i <<" is empty.\n";
            } else {
                size_t nsets = levels[i].vertices.size();
                size_t nfuncs = 0;
                for (Vertices::const_iterator vi=levels[i].vertices.begin(); vi!=levels[i].vertices.end(); ++vi)
                    nfuncs += (*vi)->functions.size();
                o <<"partition forest level " <<i
                  <<" contains " <<nfuncs <<" function" <<(1==nfuncs?"":"s")
                  <<" in " <<nsets <<" set" <<(1==nsets?"":"s") <<"\n";
                o <<"  the following input was used to generate " <<(1==nsets?"this set":"these sets") <<":\n";
                o <<StringUtility::prefixLines(levels[i].inputs.toString(), "    ");
                int setno = 1;
                for (Vertices::const_iterator vi=levels[i].vertices.begin(); vi!=levels[i].vertices.end(); ++vi, ++setno) {
                    Vertex *vertex = *vi;
                    const Functions &functions = vertex->functions;
                    o <<"  set #" <<setno
                      <<" contains " <<vertex->functions.size() <<" function" <<(1==vertex->functions.size()?"":"s") <<":\n";
                    for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
                        SgAsmFunction *func = *fi;
                        o <<"    " <<StringUtility::addrToString(func->get_entry_va()) <<" <" <<func->get_name() <<">\n";
                    }
                    o <<"    whose output was: {";
                    for (OutputValues::const_iterator oi=vertex->outputs.begin(); oi!=vertex->outputs.end(); ++oi)
                        o <<" " <<*oi;
                    o <<" }\n";
                }
            }
        }
    }

    /** Number the vertices of the forest. */
    void renumber_vertices() {
        size_t id=0;
        for (size_t i=0; i<levels.size(); ++i) {
            for (Vertices::const_iterator vi=levels[i].vertices.begin(); vi!=levels[i].vertices.end(); ++vi)
                (*vi)->id = id++;
        }
    }

    /** Dump the output sets to the DBMS. (FIXME: dumping SQL to a file instead; see dump())*/
    void dump_outputsets(std::ostream &dbc) const {
        for (size_t i=0; i<levels.size(); ++i) {
            for (Vertices::const_iterator vi=levels[i].vertices.begin(); vi!=levels[i].vertices.end(); ++vi) {
                dbc <<"insert into outputsets (id) values (" <<(*vi)->id <<");\n";
                for (OutputValues::const_iterator ni=(*vi)->outputs.begin(); ni!=(*vi)->outputs.end(); ++ni)
                    dbc <<"insert into outputvalues (outputset_id, val) values (" <<(*vi)->id <<", " <<*ni <<");\n";
            }
        }
    }

    /** Dump inputsets to the DBMS. (FIXME: dumping SQL to a file instead; see dump()) */
    void dump_inputsets(std::ostream &dbc) const {
        for (size_t i=0; i<levels.size(); ++i) {
            dbc <<"insert into inputsets (id) values(" <<i <<");\n";
            const std::vector<uint64_t> &pointers = levels[i].inputs.get_pointers();
            for (size_t j=0; j<pointers.size(); ++j)
                dbc <<"insert into inputvalues (inputset_id, vtype, pos, val)"
                    <<" values (" <<i <<", 'P', " <<j <<", " <<pointers[j] <<");\n";
            const std::vector<uint64_t> &integers = levels[i].inputs.get_integers();
            for (size_t j=0; j<integers.size(); ++j)
                dbc <<"insert into inputvalues (inputset_id, vtype, pos, val)"
                    <<" values (" <<i <<", 'N', " <<j <<", " <<integers[j] <<");\n";
        }
    }

    /** Dump an entire forest to a DBMS.
     *
     *  FIXME: This should really be using something like ODBC, but ROSE is only configured to use specific DBMS like SQLite3
     *  or MySQL. Therefore, since we don't need to do any queries, we'll just generate straight SQL as text (the sqlite3x API
     *  converts all numeric values to text anyway, so there's not really any additional slowdown or loss of precision).
     *  Therefore, the @p user_name and @p password arguments are unused, and @p server_name is the name of the file to which
     *  the SQL statements are written. */
    void dump(const std::string &server_name, const std::string &user_name, const std::string &password) {
        std::ofstream dbc(server_name.c_str());
        renumber_vertices();
        dump_outputsets(dbc);
        dump_inputsets(dbc);

        Vertices leaves = get_leaves();
        size_t leafnum = 0;

        for (Vertices::const_iterator vi=leaves.begin(); vi!=leaves.end(); ++vi, ++leafnum) {
            Vertex *vertex = *vi;
            const Functions &functions = vertex->get_functions();
            dbc <<"insert into simsets (id) values (" <<leafnum <<");\n";

            for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi)
                dbc <<"insert into functions (entry_va, simset_id) values (" <<(*fi)->get_entry_va() <<", " <<leafnum <<");\n";
            for (Vertex *v=vertex; v; v=v->parent)
                dbc <<"insert into inoutpairs (simset_id, inputset_id, outputset_id) values ("
                    <<leafnum <<", " <<v->get_level() <<", " <<v->id <<");\n";
        }
    }
        
    std::string toString() const {
        std::ostringstream ss;
        print(ss);
        return ss.str();
    }
};
        
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
    PartitionForest partition;          /**< Partitioning of functions into similarity sets. */
    enum { MAX_ITERATIONS = 10 };       /**< Maximum number of times we run the functions; max number of input sets. */
    enum { MAX_SIMSET_SIZE = 3 };       /**< Any similarity set containing more than this many functions will be partitioned. */

public:
    CloneDetector(RSIM_Thread *thread): thread(thread) {}

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
            m->mesg("%s: performing pointer detection analysis for \"%s\" at 0x%08"PRIx64,
                    name, (*fi)->get_name().c_str(), (*fi)->get_entry_va());
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

    // Randomly choose a set of input values. The set will consist of the specified number of non-pointers and pointers. The
    // non-pointer values are chosen randomly, but limited to a certain range.  The pointers are chosen randomly to be null or
    // non-null and the non-null values each have one page allocated via simulated mmap() (i.e., the non-null values themselves
    // are not actually random).
    InputValues choose_inputs(size_t nintegers, size_t npointers) {
        static unsigned integer_modulus = 256;  // arbitrary;
        static unsigned nonnull_denom = 3;      // probability of a non-null pointer is 1/N
        CloneDetection::InputValues inputs;
        for (size_t i=0; i<nintegers; ++i)
            inputs.add_integer(rand() % integer_modulus);
        for (size_t i=0; i<npointers; ++i)
            inputs.add_pointer(rand()%nonnull_denom ? 0 : allocate_page());
        return inputs;
    }

    // Run a single function, look at its outputs, and insert it into the correct place in the PartitionForest
    void insert_function(SgAsmFunction *func, InputValues &inputs, CloneDetection::PointerDetector &pointers,
                         PartitionForest &partition, PartitionForest::Vertex *parent) {
        CloneDetection::Outputs<RSIM_SEMANTICS_VTYPE> *outputs = fuzz_test(func, inputs, pointers);
        OutputValues concrete_outputs = outputs->get_values();
        partition.insert(func, concrete_outputs, parent);
    }

    // Analyze a single function by running it with the specified inputs and collecting its outputs. */
    CloneDetection::Outputs<RSIM_SEMANTICS_VTYPE> *fuzz_test(SgAsmFunction *function, CloneDetection::InputValues &inputs,
                                                             const CloneDetection::PointerDetector &pointers) {
        RSIM_Process *proc = thread->get_process();
        RTS_Message *m = thread->tracing(TRACE_MISC);
        m->mesg("==========================================================================================");
        m->mesg("%s: fuzz testing function \"%s\" at 0x%08"PRIx64, name, function->get_name().c_str(), function->get_entry_va());

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
        }

        // Gather the function's outputs before restoring machine state.
        bool verbose = true;
        CloneDetection::Outputs<RSIM_SEMANTICS_VTYPE> *outputs = thread->policy.get_outputs(verbose);
        thread->init_regs(saved_regs);
        proc->mem_transaction_rollback(name);
        return outputs;
    }

    // Run each function from the specified set of functions in order to produce an output set for each function.  Then insert
    // the functions into the bottom of the specified PartitionForest.  This runs one iteration of partitioning.
    void partition_functions(RTS_Message *m, PartitionForest &partition,
                             const Functions &functions, PointerDetectors &pointers,
                             InputValues &inputs, PartitionForest::Vertex *parent) {
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
            partition.insert(*fi, output_values, parent);
        }
    }

    // Detect functions that are semantically similar by running multiple iterations of partition_functions().
    void analyze() {
        RTS_Message *m = thread->tracing(TRACE_MISC);
        Functions functions = find_functions(m, thread->get_process());
        PointerDetectors pointers = detect_pointers(m, thread, functions);
        PartitionForest partition;
        while (partition.nlevels()<MAX_ITERATIONS) {
            InputValues inputs = choose_inputs(3, 3);
            size_t level = partition.new_level(inputs);
            m->mesg("####################################################################################################");
            m->mesg("%s: fuzz testing %zu function%s at level %zu", name, functions.size(), 1==functions.size()?"":"s", level);
            m->mesg("%s: using these input values:\n%s", name, inputs.toString().c_str());

            if (0==level) {
                partition_functions(m, partition, functions, pointers, inputs, NULL);
            } else {
                const PartitionForest::Vertices &parent_vertices = partition.vertices_at_level(level-1);
                for (PartitionForest::Vertices::const_iterator pvi=parent_vertices.begin(); pvi!=parent_vertices.end(); ++pvi) {
                    PartitionForest::Vertex *parent_vertex = *pvi;
                    if (parent_vertex->functions.size()>MAX_SIMSET_SIZE)
                        partition_functions(m, partition, parent_vertex->functions, pointers, inputs, parent_vertex);
                }
            }

            // If the new level doesn't contain any vertices then we must not have needed to repartition anything and we're all
            // done.
            if (partition.vertices_at_level(level).empty())
                break;
        }

        m->mesg("==========================================================================================");
        m->mesg("%s: The entire partition forest follows...", name);
        m->mesg("%s", StringUtility::prefixLines(partition.toString(), std::string(name)+": ").c_str());

        m->mesg("==========================================================================================");
        m->mesg("%s: Final function similarity sets are:", name);
        PartitionForest::Vertices leaves = partition.get_leaves();
        size_t setno=0;
        for (PartitionForest::Vertices::iterator vi=leaves.begin(); vi!=leaves.end(); ++vi, ++setno) {
            PartitionForest::Vertex *leaf = *vi;
            const Functions &functions = leaf->get_functions();
            m->mesg("%s:   set #%zu at level %zu has %zu function%s:",
                    name, setno, leaf->get_level(), functions.size(), 1==functions.size()?"":"s");
            for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi)
                m->mesg("%s:     0x%08"PRIx64" <%s>", name, (*fi)->get_entry_va(), (*fi)->get_name().c_str());
        }

        m->mesg("%s: dumping final similarity sets to clones.sql", name);
        partition.dump("clones.sql", "NO_USER", "NO_PASSWD");
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
