// Example showing how to run another semantic domain in parallel with the main simulator domain.

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "x86InstructionSemantics.h"
#include "IntervalSemantics.h"
#include "threadSupport.h"

using namespace rose;
using namespace rose::BinaryAnalysis::InstructionSemantics;

class IntervalAnalysis: public RSIM_Callbacks::InsnCallback {
protected:
    const char *name;           // name to use in diagnostic output
    rose_addr_t when;           // when to trigger
    bool triggered;             // has the analysis been triggered yet?

    typedef IntervalSemantics::Policy<> Policy;
    typedef X86InstructionSemantics<Policy, IntervalSemantics::ValueType> Semantics;
    Policy policy;
    Semantics semantics;

protected:
    // convert a partial symbolic semantics value to an interval semantics value
    template<size_t nBits>
    static IntervalSemantics::ValueType<nBits> convert(const PartialSymbolicSemantics::ValueType<nBits> &val) {
        return val.is_known() ? IntervalSemantics::ValueType<nBits>(val.known_value()) : IntervalSemantics::ValueType<nBits>();
    }

    // initialize interval semantics registers from partial symbolic semantics
    void initialize_register_intervals(RSIM_Thread *t) {
        IntervalSemantics::State<> &dst = policy.get_state();
        RSIM_Semantics::InnerState<> &src = t->policy.get_concrete_state();
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

public:
    IntervalAnalysis(rose_addr_t when): name("IntervalAnalysis"), when(when), triggered(false), semantics(policy) {}

    virtual IntervalAnalysis *clone() /*overrides*/ { return this; }

    virtual bool operator()(bool enabled, const Args &args) /*overrides*/ {
        if (enabled) {
            if (!triggered && args.insn->get_address()==when) {
                triggered = true;
                initialize_register_intervals(args.thread);
            }
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(args.insn);
            if (triggered && insn) {
                RTS_Message *m = args.thread->tracing(TRACE_MISC);
                m->mesg("%s: %s", name, unparseInstructionWithAddress(insn).c_str());
                semantics.processInstruction(insn);
                std::ostringstream ss; ss <<policy;
                m->mesg("%s", ss.str().c_str());
            }
        }
        return enabled;
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
    sim.install_callback(new IntervalAnalysis(sim.get_process()->get_ep_start_va()), RSIM_Callbacks::BEFORE, true);
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
