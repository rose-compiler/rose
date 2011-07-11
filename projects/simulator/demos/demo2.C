/* Demonstrates how to write a (very simple, naive) sign analysis.
 *
 * Run the program like this:
 *    $ demo2 ./demo2input 2>&1 |cat
 *
 * The pipe to "cat" is necessary only if you want to actually see the instructions.
 *
 *
 *
 * The analysis itself is defined in the SignAnalysisExample namespace (see SignAnalysisExample.h).  The analysis values are
 * some combination of zero, negative, and positive.  Since memory addresses are in the same domain as values, we have three
 * possible memory locations. This is insufficient to determine the address of the next instruction (the EIP register), so we
 * must run the SignAnalysisExample in tandem with some other analysis.  The other analysis provides EIP values that we use to
 * fetch instructions to feed to the sign analysis.
 *
 * This is meant to be a VERY SIMPLE example.  Therefore, the other analysis (that provides EIP values) is simply the simulator
 * itself.  In other words, the sign analysis occurs in synchrony with the simulation of the specimen.  This is pretty useless
 * since we could just get the signs from the simulator's concrete state, but it serves its purpose as an example of how to
 * write another analysis.
 *
 * There are several alternatives to running synchronously.  For instance, if we know the current instruction then we can
 * perform sign analysis on that instruction and all following instructions as long as we can determine a unique control-flow
 * successor.  I.e., we can do the analysis on following instructions of the same basic block, plus subsequent basic blocks as
 * long as we can determine branch targets at the end of each block.  Another demo shows how the sign analysis can be used in
 * tandem with symbolic analysis.
 */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "SignAnalysisExample.h"

/** Runs sign analysis.
 *
 *  Since this demo runs the sign analysis in synchrony with the simulation, all we need to do is attach the sign analysis to
 *  the simulator's instruction execution.  This is done with an instruction callback. */
class SynchronousSignAnalysis: public RSIM_Callbacks::InsnCallback {
private:
    /* The policy encapsulates all the information about the analysis:  the types of values used by the analysis, the current
     * state of the analysis, and the operators defined over the values and state. */
    SignAnalysisExample::Policy policy;

    /* The policy is attached to an instruction semantics class.  When the instruction semantics class "executes" an
     * instruction it calls methods in the policy, resulting in updates to the policy state. */
    X86InstructionSemantics<SignAnalysisExample::Policy, SignAnalysisExample::ValueType> semantics;

public:
    SynchronousSignAnalysis(): semantics(policy) {}

    /* Every thread needs its own copy of this callback, otherwise the instruction streams from two or more threads would end
     * up interfering with each other. */
    virtual SynchronousSignAnalysis *clone() {
        SynchronousSignAnalysis *retval = new SynchronousSignAnalysis;
        retval->policy = policy;
        return retval;
    }

    /* The instruction is fed to the sign analysis, updating its state. */
    virtual bool operator()(bool enabled, const Args &args) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(args.insn);
        if (enabled && insn) {
            semantics.processInstruction(insn);
            std::cerr <<policy;
        }
        return enabled;
    }
};

int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    /* Turn on instruction tracing regardless of command-line settings.  Note: This interface is not ideal; we're planning to
     * add methods to adjust tracing in a more friendly manner. */
    static const char *extras[] = {"INTERNAL", "--debug=insn"};
    sim.configure(2, (char**)extras, NULL);

    /* Install an instruction callback that does the sign analysis.  We could use the simpler install_callback() method, but we
     * want this callback to trigger _after_ each instruction in order to make the output more readable. */
    sim.get_callbacks().add_insn_callback(RSIM_Callbacks::AFTER, new SynchronousSignAnalysis);

    /* Run the simulation.  This is pretty much boiler plate. */
    sim.exec(argc-n, argv+n);
    sim.activate();
    sim.main_loop();
    sim.deactivate();
    sim.describe_termination(stderr);
    sim.terminate_self(); // never returns
    return 0;
}



#else

int main(int argc, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif
