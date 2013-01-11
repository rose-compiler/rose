#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "SymbolicSemantics.h"
#include "YicesSolver.h"
#include "BinaryPointerDetection.h"

using namespace BinaryAnalysis::InstructionSemantics;

// If this symbol is undefined then the simulator will not forward its own signals to the specimen, making it easier to kill
// the specimen in some cases.  See its use in main().
#undef PRODUCTION

// Choose an arbitrary concrete value for the stack pointer register (ESP) in order to simplify memory addresses.  This makes
// it easier for the output from the PtrAnalysis to be used by the OracleAnalysis.
static const rose_addr_t analysis_stack_va = 0xb0000000ul;

enum Verbosity {VB_SILENT, VB_MINIMAL, VB_SOME, VB_LOTS};


/******************************************************************************************************************************
 *                                      Values used for Pointer and Oracle Analysis
 ******************************************************************************************************************************/

// This is a stub and is here so that if we need to add something to the values we can do so quite easily.  It also allows us
// to define conversions between ValueTypes defined for different instruction semantics name spaces.
template<size_t nBits>
class ValueType: public SymbolicSemantics::ValueType<nBits> {
public:
    ValueType(std::string comment="")
        : SymbolicSemantics::ValueType<nBits>(comment) {}
    ValueType(const SymbolicSemantics::ValueType<nBits> &other)
        : SymbolicSemantics::ValueType<nBits>(other) {}
    ValueType(uint64_t n, std::string comment="")
        : SymbolicSemantics::ValueType<nBits>(n, comment) {}
    explicit ValueType(const SymbolicSemantics::TreeNodePtr &node)
        : SymbolicSemantics::ValueType<nBits>(node) {}
    ValueType(const PartialSymbolicSemantics::ValueType<nBits> &other, std::string comment="") {
        set_expression(other.is_known() ?
                       InsnSemanticsExpr::LeafNode::create_integer(nBits, other.known_value(), comment) :
                       InsnSemanticsExpr::LeafNode::create_variable(nBits, comment));
    }
};

/******************************************************************************************************************************
 *                                              Pointer Detection
 ******************************************************************************************************************************/


typedef BinaryAnalysis::PointerAnalysis::PointerDetection<RSIM_Process, ValueType> PointerDetector;

/******************************************************************************************************************************
 *                                              Memory Oracle Analysis
 ******************************************************************************************************************************/

template<template <size_t> class ValueType>
class OracleState: public SymbolicSemantics::State<ValueType> {
public:
    OracleState& operator=(const SymbolicSemantics::State<ValueType> &other) {
        *dynamic_cast<SymbolicSemantics::State<ValueType>*>(this) = other;
        return *this;
    }
};

template<
    template<template<size_t> class ValueType> class State,
    template<size_t> class ValueType>
class OraclePolicy: public SymbolicSemantics::Policy<State, ValueType>
{
public:
    typedef typename       SymbolicSemantics::Policy<State, ValueType>  super;
    typedef typename State<ValueType>::Memory Memory;
    const PointerDetector &pointers;

    OraclePolicy(const SymbolicSemantics::Policy<SymbolicSemantics::State, ValueType> &init_policy,
                 const PointerDetector &pointers)
        : pointers(pointers) {
        this->cur_state = init_policy.get_state();
    }

    // Provide a memory oracle.  When reading memory we could have two approaches.  A simple approach would be to first do a
    // readMemory() from the superclass and if the obtained value is symbolic, follow it with a writeMemory() to the
    // superclass.  However, this has the problem that a readMemory() is also responsible for updating the initial memory
    // state (it's done this way so we don't have to store an initial value for every possible memory address--we only store
    // the values for memory locations that are read without having first been written.)  The initial state ends up containing
    // symbolic values rather than the concrete values we would have liked.
    //
    // So we use a different approach.  Instead of using the policy's readMemory() interface, we call the
    // SymbolicSemantics::Policy::mem_read() method and supply a default value.  The drawback is that we need to compute the
    // default value for every call to mem_read() since we don't know before hand whether the default value will be needed.
    template<size_t nBits>
    ValueType<nBits>
    readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> &cond) {
        ValueType<nBits> dflt = pointers.is_pointer(addr) ? random_pointer<nBits>() : random_integer<nBits>();
        return super::template mem_read(this->get_state(), addr, &dflt);
    }

    // Return a random pointer
    template<size_t nBits>
    ValueType<nBits>
    random_pointer() const {
        ValueType<nBits> retval = this->template number<nBits>(rand() % (1ul << nBits));
        return retval;
    }

    // Return a random integer
    template<size_t nBits>
    ValueType<nBits>
    random_integer() const {
        static const uint64_t limit = 100; // arbitrary
        ValueType<nBits> retval = this->template number<nBits>(rand() % limit);
        return retval;
    }
};

class OracleAnalysis {
public:
    typedef OraclePolicy<OracleState, ValueType> Policy;
    typedef X86InstructionSemantics<Policy, ValueType> Semantics;
    typedef SymbolicSemantics::Policy<SymbolicSemantics::State, ValueType> InitialPolicy;

    InitialPolicy initial_policy;

    OracleAnalysis(const InitialPolicy &p): initial_policy(p) {}

    // Throws:
    //   Disassembler::Exception when it can't disassemble something, probably because we supplied a bugus value for
    //   the instruction pointer.
    size_t analyze(rose_addr_t analysis_addr, const PointerDetector &pointers, RSIM_Thread *thread) {
        static const size_t limit = 200;        // max number of instructions to process
        size_t ninsns = 0;                      // number of instructions processed
        Policy policy(initial_policy, pointers);
        Semantics semantics(policy);

        try {
            // Initialize the policy for the first simulated instruction.
            policy.writeRegister(semantics.REG_EIP, policy.number<32>(analysis_addr));

            // Simulate some instructions
            for (/*void*/; ninsns<limit && policy.readRegister<32>(semantics.REG_EIP).is_known(); ++ninsns) {
                rose_addr_t va = policy.readRegister<32>(semantics.REG_EIP).known_value();
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(thread->get_process()->get_instruction(va));
                semantics.processInstruction(insn);
            }
        } catch (const Disassembler::Exception &e) {
        } catch (const Semantics::Exception &e) {
        } catch (const Policy::Exception &e) {
        }
        return ninsns;
    }
};


/******************************************************************************************************************************
 *                                              Top-level analysis
 *
 * This callback just waits for the EIP to reach a certain value and then fires off the pointer and oracle analyses.
 ******************************************************************************************************************************/

class AnalysisTrigger: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t trigger_addr, analysis_addr;
    size_t randomize, niters;
    Verbosity verbosity;
    ExtentMap limits; // limits for choosing random analysis addresses; empty means no limits

    AnalysisTrigger(rose_addr_t trigger_addr, rose_addr_t analysis_addr, size_t randomize, const ExtentMap &limits,
                    size_t niters, Verbosity verbosity)
        : trigger_addr(trigger_addr), analysis_addr(analysis_addr), randomize(randomize), niters(niters),
          verbosity(verbosity), limits(limits) {}

    // This analysis is intended to run in a single thread, so clone is a no-op.
    virtual AnalysisTrigger *clone() { return this; }

    // Given a function, figure out what part of the address space its instructions occupy.
    ExtentMap function_extent(SgAsmFunction *func) {
        struct: public AstSimpleProcessing {
            ExtentMap extents;
            void visit(SgNode *node) {
                SgAsmInstruction *insn = isSgAsmInstruction(node);
                if (insn)
                    extents.insert(Extent(insn->get_address(), insn->get_size()));
            }
        } t;
        t.traverse(func, preorder);
        return t.extents;
    }

    // The actual analysis, triggered when we reach the specified execution address...
    virtual bool operator()(bool enabled, const Args &args) {
        static const bool limit_to_function = true;
        if (enabled && args.insn->get_address()==trigger_addr) {
            static const char *name = "AnalysisTrigger";
            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            m->mesg("%s triggered\n", name);

            // Show a memory map to aid the user in choosing address limits in future runs.
            args.thread->get_process()->mem_showmap(m, "process memory map:\n");

            // Disassemble if necessary.  The only reason we would need to do this is if we need to choose random addresses for
            // the analysis (fast, simple disassembly) and/or if we need to know how instructions are related to functions.
            std::vector<SgAsmInstruction*> insns;
            if (randomize>0 || limit_to_function) {
                m->mesg("%s: disassembling the whole program...", name);
                SgAsmBlock *gblk = args.thread->get_process()->disassemble(!limit_to_function);
                insns = SageInterface::querySubTree<SgAsmInstruction>(gblk);
                m->mesg("%s: disassembled %zu instructions", name, insns.size());
#if 0
                const char *listing_name = "x-prog.lst";
                m->mesg("%s: generating assembly listing in \"%s\"", name, listing_name);
                std::ofstream listing(listing_name);
                AsmUnparser().unparse(listing, gblk);
#endif
                if (!limits.empty()) {
                    std::ostringstream ss; ss<<limits;
                    m->mesg("%s: limiting instructions to addresses: %s\n", name, ss.str().c_str());
                    std::vector<SgAsmInstruction*> tmp = insns;
                    insns.clear();
                    for (size_t i=0; i<tmp.size(); ++i) {
                        if (limits.contains(Extent(tmp[i]->get_address(), tmp[i]->get_size())))
                            insns.push_back(tmp[i]);
                    }
                    m->mesg("%s: %zu instructions left from which to choose\n", name, insns.size());
                }
            }

            // Create an initial state.  OracleAnlaysis accepts a SymbolicSemantics::Policy as an initializer.
            typedef SymbolicSemantics::Policy<SymbolicSemantics::State, ValueType> InitialPolicy;
            typedef X86InstructionSemantics<InitialPolicy, ValueType> InitialSemantics;
            InitialPolicy init_policy;
            init_policy.get_state().registers.gpr[x86_gpr_sp] = ValueType<32>(analysis_stack_va, "esp@init");


            // Run the PtrAnalysis and OracleAnalysis repeatedly with different starting addresses
            for (size_t n=0; n<std::max((size_t)1, randomize); ++n) {

                // Choose an analysis address
                SgAsmInstruction *insn = NULL;
                if (n>0 || (randomize>0 && 0==analysis_addr)) {
                    insn = insns[rand() % insns.size()];
                    analysis_addr = insn->get_address();
                } else {
                    try {
                        insn = args.thread->get_process()->get_instruction(analysis_addr);
                    } catch (...) {
                    }
                }

                // Limit the range of addresses that will be considered during the analysis
                ExtentMap addrspc;
                if (limit_to_function) {
                    SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(insn);
                    if (func) {
                        addrspc = function_extent(func);
                    } else {
                        m->mesg("%s: cannot limit analysis to a single function at insn at 0x%08"PRIx64, name, analysis_addr);
                    }
                } else if (!limits.empty()) {
                    addrspc = limits;
                }

                // Do one pointer analysis
                SMTSolver *solver = NULL;
                PointerDetector ptr_detector(args.thread->get_process(), solver);
                ptr_detector.analyze(analysis_addr, addrspc);
                const PointerDetector::Pointers &pointers = ptr_detector.get_pointers();
                std::cout <<"Pointers discovered by PtrAnalysis:\n";
                for (PointerDetector::Pointers::const_iterator pi=pointers.begin(); pi!=pointers.end(); ++pi) {
                    const PointerDetector::Pointer &pointer = *pi;
                    if (pointer.type & BinaryAnalysis::PointerAnalysis::DATA_PTR)
                        std::cout <<"data ";
                    if (pointer.type & BinaryAnalysis::PointerAnalysis::CODE_PTR)
                        std::cout << "code ";
                    std::cout <<"pointer at " <<pointer.address <<"\n";
                }

                // Do a bunch of memory oracle analyses
                for (size_t iter=0; iter<niters; ++iter) {

                    // Initialize some registers
                    SymbolicSemantics::State<ValueType> &state = init_policy.get_state();
                    for (size_t i=0; i<state.registers.n_gprs; ++i) {
                        if (i!=x86_gpr_sp && i!=x86_gpr_bp)
                            state.registers.gpr[i] = ValueType<32>((uint64_t)rand() & 0xffffffffull);
                    }
                    for (size_t i=0; i<state.registers.n_segregs; ++i)
                        state.registers.segreg[i] = args.thread->policy.get_concrete_state().registers.segreg[i];
                    for (size_t i=0; i<state.registers.n_flags; ++i)
                        state.registers.flag[i] = ValueType<1>((uint64_t)rand() & 1ull);

                    m->mesg("%s: MemoryOracle at virtual address 0x%08"PRIx64" (%zu of %zu), iteration %zu",
                            name, analysis_addr, n+1, std::max(randomize, (size_t)1), iter);
                    size_t ninsns = OracleAnalysis(init_policy).analyze(analysis_addr, ptr_detector, args.thread);
                    m->mesg("%s:   MemoryOracle processed %zu instruction%s", name, ninsns, 1==ninsns?"":"s");
                }
            }
            throw this; // terminate simulation
        }
        return enabled;
    }
};

int main(int argc, char *argv[], char *envp[])
{
    // Parse (and remove) switches intended for the analysis.
    std::string trigger_func, analysis_func;
    rose_addr_t trigger_va=0, analysis_va=0;
    size_t randomize=0, niters=1;
    Verbosity verbosity=VB_MINIMAL;
    ExtentMap limits;

    for (int i=1; i<argc; i++) {
        if (!strncmp(argv[i], "--trigger=", 10)) {
            // name or address of function triggering analysis
            char *rest;
            trigger_va = strtoull(argv[i]+10, &rest, 0);
            if (*rest) {
                trigger_va = 0;
                trigger_func = argv[i]+10;
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--random=", 9)) {
            // number of analyses (i.e., number of analysis starting addresses)
            randomize = strtoul(argv[i]+9, NULL, 0);
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--limits=", 9)) {
            // when --random is specified, limit instructions to those which are contained in the specified part of the address
            // range.  The value of this switch should be a comma-separated list of addresses appearing in pairs consisting of
            // the low limit and high limit (inclusive).
            char *s=argv[i]+9, *rest;
            while (*s) {
                rose_addr_t lo=strtoull(s, &rest, 0);
                s = *rest ? rest+1 : rest;
                rose_addr_t hi=strtoull(s, &rest, 0);
                s = *rest ? rest+1 : rest;
                if (hi<lo) {
                    std::cerr <<argv[0] <<": --limits switch should have pairs of inclusive address limits.\n";
                    return 1;
                }
                limits.insert(Extent::inin(lo, hi));
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--analysis=", 11)) {
            // name or address of function to analyze.  If the name is "oep" then trigger on the original entry point.
            char *rest;
            analysis_va = strtoull(argv[i]+11, &rest, 0);
            if (*rest) {
                analysis_va = 0;
                analysis_func = argv[i]+11;
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--iterations=", 13)) {
            // number of iterations of each analysis
            niters = strtoul(argv[i]+13, NULL, 0);
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--verbosity=", 12)) {
            if (!strcmp(argv[i]+12, "silent")) {
                verbosity = VB_SILENT;
            } else if (!strcmp(argv[i]+12, "minimal")) {
                verbosity = VB_MINIMAL;
            } else if (!strcmp(argv[i]+12, "some")) {
                verbosity = VB_SOME;
            } else if (!strcmp(argv[i]+12, "lots")) {
                verbosity = VB_LOTS;
            } else {
                std::cerr <<argv[0] <<": --verbosity should be silent, minimal, some, or lots\n";
                return 1;
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        }
    }
    if (trigger_func.empty() && 0==trigger_va) {
        std::cerr <<argv[0] <<": --trigger=NAME_OR_ADDR is required\n";
        return 1;
    }
    if (analysis_func.empty() && 0==analysis_va && 0==randomize) {
        std::cerr <<argv[0] <<": --analysis or --random switch is required\n";
        return 1;
    }

    // Parse switches intended for the simulator.
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    // Parse the ELF container so we can get to the symbol table.  This is normal ROSE static analysis.
    char *rose_argv[4];
    int rose_argc=0;
    rose_argv[rose_argc++] = argv[0];
    rose_argv[rose_argc++] = strdup("-rose:read_executable_file_format_only");
    rose_argv[rose_argc++] = argv[n];
    rose_argv[rose_argc] = NULL;
    SgProject *project = frontend(rose_argc, rose_argv);

    // Find the address of "main" and analysis functions.
    if (!trigger_func.empty() && trigger_func!="oep") {
        trigger_va = RSIM_Tools::FunctionFinder().address(project, trigger_func);
        if (0==trigger_va) {
            std::cerr <<argv[0] <<": could not find trigger function: " <<trigger_func <<"\n";
            return 1;
        }
    }
    if (!analysis_func.empty()) {
        analysis_va = RSIM_Tools::FunctionFinder().address(project, analysis_func);
        if (0==analysis_va) {
            std::cerr <<argv[0] <<": could not find analysis function: " <<analysis_func <<"\n";
            return 1;
        }
    }

    // Register the analysis callback.
    AnalysisTrigger trigger(trigger_va, analysis_va, randomize, limits, niters, verbosity);
    sim.install_callback(&trigger);

    // The rest is normal boiler plate to run the simulator, except we'll catch the Analysis to terminate the simulation early
    // if desired.
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);
    sim.exec(argc-n, argv+n);
#ifdef PRODUCTION
    sim.activate();
#endif
    if (trigger_func=="oep")
        trigger.trigger_addr = sim.get_process()->get_ep_orig_va();
    try {
        sim.main_loop();
    } catch (AnalysisTrigger*) {
        std::cerr <<"analysis finsished; simulation terminated.\n";
    }
#ifdef PRODUCTION
    sim.deactivate();
#endif
    return 0;
}





#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
