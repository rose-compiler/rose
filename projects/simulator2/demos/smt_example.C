#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "x86InstructionSemantics.h"
#include "SymbolicSemantics.h"
#include "YicesSolver.h"

using namespace rose::BinaryAnalysis::InstructionSemantics;

class SymbolicAnalysis: public RSIM_Callbacks::InsnCallback {
protected:
    typedef SymbolicSemantics::Policy<> Policy;
    typedef X86InstructionSemantics<Policy, SymbolicSemantics::ValueType> Semantics;
    const char *name;           // name to use in diagnostic output
    rose_addr_t when;           // when to trigger
    bool triggered;             // has the analysis been triggered yet?
    Policy policy;
    Semantics semantics;
    rose::BinaryAnalysis::YicesSolver yices;

public:
    SymbolicAnalysis(rose_addr_t when)
        : name("SymbolicAnalysis"), when(when), triggered(false), policy(&yices), semantics(policy) {
#if 0
        yices.set_linkage(YicesSolver::LM_LIBRARY); // about 7x faster than LM_EXECUTABLE, but limited debugging
#else
        yices.set_linkage(rose::BinaryAnalysis::YicesSolver::LM_EXECUTABLE);
        //yices.set_debug(stderr);
#endif
    }

    virtual SymbolicAnalysis *clone() /*overrides*/ { return this; }

    virtual bool operator()(bool enabled, const Args &args) /*overrides*/ {
        if (enabled) {
            if (!triggered && args.insn->get_address()==when) {
                triggered = true;
                initialize_state(args.thread);
            }
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(args.insn);
            if (triggered && insn) {
                RTS_Message *m = args.thread->tracing(TRACE_MISC);
                m->mesg("%s: %s", name, unparseInstructionWithAddress(insn).c_str());
                policy.get_state().registers.ip = SymbolicSemantics::ValueType<32>(insn->get_address());
                semantics.processInstruction(insn);

                rose::BinaryAnalysis::SMTSolver::Stats smt_stats = yices.get_stats();
                m->mesg("%s: mem-cell list size: %zu elements\n", name, policy.get_state().memory.cell_list.size());
                m->mesg("%s: SMT stats: ncalls=%zu, input=%zu bytes, output=%zu bytes\n",
                        name, smt_stats.ncalls, smt_stats.input_size, smt_stats.output_size);
                yices.reset_stats();

#if 0
                std::ostringstream ss; ss <<policy;
                m->mesg("%s", ss.str().c_str());
#endif
            }
        }
        return enabled;
    }

protected:
    // convert a concrete value from the simulator's state to a symbolic value for our own state.
    template<size_t nBits>
    static SymbolicSemantics::ValueType<nBits> convert(const RSIM_SEMANTICS_OUTER_0_VTYPE<nBits> &val) {
        assert(val.is_known()); // must be concrete
        return SymbolicSemantics::ValueType<nBits>(val.known_value());
    }

    // Initialize our state from the simulator's state. We only initialize registers, not memory.  The simulator's memory can
    // have millions of defined addresses and adding them all to our state in the form of a big SMT array or function
    // expression would be prohibitively expensive in both space and time.
    void initialize_state(RSIM_Thread *t) {
        SymbolicSemantics::State<> &dst = policy.get_state();
        RSIM_SEMANTICS_OUTER_0_STATE<RSIM_SEMANTICS_OUTER_0_VTYPE> &src = t->policy.get_concrete_state();
        assert(dst.registers.n_gprs==src.registers.n_gprs);
        for (size_t i=0; i<dst.registers.n_gprs; ++i)
            dst.registers.gpr[i] = convert(src.registers.gpr[i]);
        assert(dst.registers.n_segregs==src.registers.n_segregs);
        for (size_t i=0; i<dst.registers.n_segregs; ++i)
            dst.registers.segreg[i] = convert(src.registers.segreg[i]);
        assert(dst.registers.n_flags==src.registers.n_flags);
        for (size_t i=0; i<dst.registers.n_flags; ++i)
            dst.registers.flag[i] = convert(src.registers.flag[i]);
    }
};

// Main program is almost standard RSIM boilerplate.  The only thing we add is the instantiation and installation of the
// SymbolicBombDetector callback.
int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);
    int n = sim.configure(argc, argv, envp);
    sim.exec(argc-n, argv+n);
    sim.install_callback(new RSIM_Tools::UnhandledInstruction); // needed for some versions of ld-linux.so
    sim.install_callback(new SymbolicAnalysis(sim.get_process()->get_ep_start_va()), RSIM_Callbacks::BEFORE, true);
    sim.main_loop();
    sim.describe_termination(stderr);
    sim.terminate_self();
    return 0;
}

#else

int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
