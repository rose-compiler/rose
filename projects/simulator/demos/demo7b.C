/* Similar to demo7 except instead of doing the disassembly immediately after the program is loaded, we wait until the original
 * (static) entry point is hit, which is after the dynamic linker has run.  This allows us to also disassemble functions that
 * were dynamically linked into the executable.
 *
 * Run this demo like this:
 *     $ demo7b ./demo7input
 *
 * Documentation can be found by running
 *     $ doxygen RSIM.dxy
 *     $ $WEB_BROWSER docs/html/index.html
 */

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"

using namespace rose::BinaryAnalysis;

class Demo: public RSIM_Callbacks::InsnCallback {
public:
    virtual Demo *clone() { return this; }
    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled) {
            RSIM_Process *p = args.thread->get_process();
            if (args.insn->get_address()==p->get_ep_orig_va()) {
                std::cerr <<"Disassembling the whole process...\n";
#if 1
                SgAsmBlock *gblock = p->disassemble(true/*fast*/);
#else
                SgAsmBlock *gblock = p->disassemble(false/*partitioned*/);
#endif
                std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(gblock);
                std::cout <<"Disassembled " <<insns.size() <<" instructions.\n";
                AsmUnparser().unparse(std::cout, gblock);
                throw this;
            }
        }
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

    /* The usual stuff */
    sim.exec(argc-n, argv+n);
    //sim.activate();
    try {
        sim.main_loop();
    } catch (const Demo*) {
    }
    //sim.deactivate();
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
