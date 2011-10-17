/* Demonstrates how to get the function that an instruction belongs to.
 *
 * There are three steps:
 *
 *   1.  You have to disassemble the instructions (ROSE's Disassembler classes) and partition them into basic blocks and
 *       functions (ROSE's Partitioner class).  The RSIM_Process::disassemble() will do all this for you and cache the results
 *       in the RSIM_Process object of the simulator.  The RSIM_Tools::MemoryDisassembler is another way to do this--it
 *       triggers RSIM_Process::disassemble() when a certain instruction is hit. If you don't run the disassembler over the
 *       whole process memory, the instruction you'll get back won't have any basic block or function information.  In other
 *       words, you can't discover basic blocks or functions if you only disassemble one instruction--you have to disassemble a
 *       whole bunch.
 *
 *   2.  You have to obtain a pointer to an instruction (SgAsmInstruction).  The best way to get a single instruction is with
 *       RSIM_Process::get_instruction(), which takes a specimen virtual address and returns, if possible, an instruction that
 *       was cached in step 1.  If an instruction isn't cached, then the disassembler is called to obtain a single instruction,
 *       and in this case the instruction won't have basic block or function information.
 *
 *       Another alternative is to write an instruction callback that will be invoked for every simulated instruction and an
 *       SgAsmInstruction will be passed as one of its arguments.  That's what we do in this example.
 *
 *   3.  Once you have an instruction pointer, you just traverse upward in the AST to find the SgAsmFunction
 *       node. There are four cases:
 *         A.  The instruction has no SgAsmFunction parent.  This can happen if you asked for an instruction that
 *             wasn't run through the partitioning process.  Perhaps you didn't call the disassembler, the disassembler didn't
 *             disassemble at that address for some reason, or the memory containing the instruction has been modified since
 *             the instruction was first disassembled.
 *         B.  The instruction's SgAsmFunction has the FUNC_LEFTOVERS bit set in the vector returned by the
 *             get_reason() method.  This means the Partitioner couldn't figure out which function this instruction belongs to,
 *             so it lumped it into a catch-all function.  Be aware that control flow analysis etc. will probably be
 *             meaningless on this function.
 *         C.  The SgAsmFunction has an empty name string.  This means that the instruction's function was detected,
 *             but the Partitioner couldn't find any information in symbol tables.  The RSIM_Process::disassemble() will only
 *             know about symbol tables in the main executable, not symbol tables in shared objects that were linked in by the
 *             dynamic linker. (We hope to remedy this).  If the function has no name, you can probably create your own name by
 *             using the function's unique entry address (see SgAsmFunction::get_entry_va()).
 *         D.  Otherwise the function exists and has a name.
 *
 * This test simply prints the name of every instruction's function if known.
 *
 * Run this demo like this:
 *     $ demo7 ./demo7input
 *
 * Documentation can be found by running
 *     $ doxygen RSIM.dxy
 *     $ $WEB_BROWSER docs/html/index.html
 */

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"

class Demo: public RSIM_Callbacks::InsnCallback {
public:
    virtual Demo *clone() { return this; }
    virtual bool operator()(bool enabled, const Args &args) {
        SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(args.insn);
        if (enabled && func!=NULL && 0==(func->get_reason() & SgAsmFunction::FUNC_LEFTOVERS) &&
            !func->get_name().empty())
            args.thread->tracing(TRACE_MISC)->mesg("in function %s", func->get_name().c_str());
        return enabled;
    }
};

int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    /* Register the Demo callback to print function names. */
    sim.install_callback(new Demo);

    /* We might need some instructions for the dynamic linker that aren't part of ROSE yet. */
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);

    /* Load the executable into memory and immediately disassemble that memory.  If you want to also disassemble the
     * dynamically linked libraries you need to wait until the linker is done, which is demonstrated in other examples. */
    sim.exec(argc-n, argv+n);
    SgAsmBlock *top = sim.get_process()->disassemble();

    /* Generate an assembly listing. It will include only the main executable and none of the dynamic objects (other than the
     * thunks with names like "malloc@plt") */
    AsmUnparser().unparse(std::cout, top);

    /* The usual stuff */
    sim.activate();
    sim.main_loop();
    sim.deactivate();
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
    

#endif
