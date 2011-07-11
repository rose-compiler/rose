/* Demonstrates how to perform sign analysis in tandem with symbolic analysis.
 *
 * Run this demo like this:
 *   $ demo4 ./demo4input
 *
 * You must have configured ROSE with "--enable-yices" and have libyices.a available.  If not, this demo will still compile but
 * you'll get a failed assertion at run time.
 *
 *
 *
 * As mentioned in an earler demo, the SignAnalysisExample must be run in tandem with some other analysis in order to figure
 * out which instructions to process -- the sign analysis itself does not calculate enough information to know the address of
 * the next instruction.  The earlier demo ran the sign analysis in synchrony with the simulator itself, which was a bit
 * useless since we could just get signs by consulting the concrete values in the simulators state.
 *
 * This demo does sign analysis over a function and uses symbolic analysis to figure out instruction addresses.  It's also a
 * bit useless because the signs can be discovered by running an SMT solver.  However, it demonstrates how one analysis can
 * drive another analysis.
 *
 * Here's what we do:
 *
 * 1. Allow the specimen to execute up to a certain point in order to resolve dynamic linking.  This demo parses the ELF file
 *    to find the address of "main" and stops when it is reached.  By executing to main(), we allow the dynamic linker to run,
 *    giving us more information about the executable.
 *
 * 2. When main is reached, we will do some symbolic analysis on the "payload" function.  This function, which normally isn't
 *    called, takes one integer argument and calculates an integer result.  Our analysis gives an expression for the result in
 *    terms of the (unknown) input argument.  In other words, an expression defining the function.
 *
 * 3. As the symbolic analysis discovers the control flow path, instructions on that path are fed into the sign analysis.  When
 *    the symbolic analysis can no longer find a unique control flow path, the analysis stops.  For simple enough functions,
 *    this will happen at the end of the function because we didn't give the analysis a return address.
 */

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "SymbolicSemantics.h"
#include "SignAnalysisExample.h"

/* Monitors the CPU instruction pointer.  When it reaches a specified value analyze the function at the specified location. */
class Analysis: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t trigger_addr, analysis_addr;

    Analysis(rose_addr_t trigger_addr, rose_addr_t analysis_addr)
        : trigger_addr(trigger_addr), analysis_addr(analysis_addr) {}

    /* This analysis is intended to run in a single thread, so clone is a no-op. */
    virtual Analysis *clone() { return this; }

    /* The actual analysis, triggered when we reach the specified execution address... */
    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && args.insn->get_address()==trigger_addr) {
            /* Create the symbolic analysis to drive the sign analysis. */
            SymbolicSemantics::Policy symbolic_policy;
            X86InstructionSemantics<SymbolicSemantics::Policy, SymbolicSemantics::ValueType> symbolic_semantics(symbolic_policy);

            /* Create the sign analysis. */
            SignAnalysisExample::Policy sign_policy;
            X86InstructionSemantics<SignAnalysisExample::Policy, SignAnalysisExample::ValueType> sign_semantics(sign_policy);

            /* Run the analyses until we can't figure out what instruction is next.  If we set things up correctly, the
             * simulation will stop when we hit the RET instruction to return from this function. */
            symbolic_policy.writeIP(SymbolicSemantics::ValueType<32>(analysis_addr));
            while (symbolic_policy.readIP().is_known()) {
                uint64_t va = symbolic_policy.readIP().known_value();
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(args.thread->get_process()->get_instruction(va));
                assert(insn!=NULL);
                symbolic_semantics.processInstruction(insn);
                sign_semantics.processInstruction(insn);
            }

            std::cout << sign_policy;
            throw this; // Optional: will exit simulator, caught in main()
        }
        return enabled;
    }
};

int main(int argc, char *argv[], char *envp[])
{
    /* Configure the simulator by parsing command-line switches. The return value is the index of the executable name in argv. */
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    /* Parse the ELF container so we can get to the symbol table. */
    char *rose_argv[4];
    int rose_argc=0;
    rose_argv[rose_argc++] = argv[0];
    rose_argv[rose_argc++] = strdup("-rose:read_executable_file_format_only");
    rose_argv[rose_argc++] = argv[n];
    rose_argv[rose_argc] = NULL;
    SgProject *project = frontend(rose_argc, rose_argv);

    /* Find the address of "main" and "payload" functions. */
    rose_addr_t main_addr = FunctionFinder().address(project, "main");
    assert(main_addr!=0);
    rose_addr_t payload_addr = FunctionFinder().address(project, "payload");
    assert(payload_addr!=0);

    /* Register the analysis callback. */
    Analysis analysis(main_addr, payload_addr);
    sim.install_callback(&analysis);

    /* The rest is pretty standard, other than we catch the exception thrown by the analysis in order to terminate the
     * simulation early. */
    sim.exec(argc-n, argv+n);
    sim.activate();
    try {
        sim.main_loop();
    } catch (Analysis*) {
    }
    sim.deactivate();
    return 0;
}




#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
