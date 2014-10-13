/* Disassembles an entire process.  This is different than disassembling an executable file since a process also has other code
 * areas such as dynamically linked libraries.
 *
 * Usage:
 *     x86dis [SWITCHES] SPECIMEN [SPECIMEN_ARGS...]
 *
 * Where SWITCHES are any of the simulator switches (see RSIM_Simulator::configure()) and/or the following:
 *     --trigger=ADDRESS
 *     --trigger=FUNCTION
 *     --trigger=oep
 *              The disassembly is triggered when the specified address is reached.  If the address is the string "oep" then
 *              the disassembly is triggered when the specimen reaches its original entry pointer--the entry address specified
 *              in the executable's main ELF file.  If the address is some other non-numeric string then it is treated as a
 *              function name and the name is looked up in the symbol tables of all the loaded ELF files.  Note that the
 *              simulator only knows about and parses ELF files that are loaded by the simulator; it does not currently parse
 *              ELF files loaded as a result of simulating the specimen's interpreter (usually the dynamic loader,
 *              ld-linux.so).
 *     --linear
 *              Organized disassembly listing by address rathern than by function.
 */

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "RSIM_Adapter.h"

using namespace rose::BinaryAnalysis;

// Callback to disassemble an entire process when a certain instruction address is reached.
class MyDisassembler: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t trigger_va;
    AsmUnparser::Organization org;

    MyDisassembler(rose_addr_t trigger_va, AsmUnparser::Organization org): trigger_va(trigger_va), org(org) {}

    virtual MyDisassembler *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && args.insn->get_address()==trigger_va) {
            args.thread->get_process()->get_simulator()->deactivate();
            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            m->mesg("disassembly triggered; disassembling now...\n");
            SgAsmBlock *gblk = args.thread->get_process()->disassemble(false); // full disassembly with partitioning
            AsmUnparser unparser;
            unparser.set_organization(org);
            unparser.unparse(std::cout, gblk);
            throw this; // to terminate specimen
        }
        return enabled;
    }
};

int
main(int argc, char *argv[], char *envp[])
{
    rose_addr_t trigger_va = 0;         // address at which disassembly is triggered
    std::string trigger_func = "oep";   // name of function at which disassembly is triggered (oep=original entry point)
    AsmUnparser::Organization org = AsmUnparser::ORGANIZED_BY_AST;

    // Parse arguments that we need for ourself.
    for (int i=1; i<argc; i++) {
        if (!strncmp(argv[i], "--trigger=", 10)) {
            // Address (or name of function) that will trigger the disassembly.  When the EIP register contains this value then
            // disassembly will run and the specimen will be terminated.
            char *rest;
            trigger_func = "";
            trigger_va = strtoull(argv[i]+10, &rest, 0);
            if (*rest) {
                trigger_va = 0;
                trigger_func = argv[i]+10;
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strcmp(argv[i], "--linear")) {
            org = AsmUnparser::ORGANIZED_BY_ADDRESS;
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        }
    }

    // Initialize the simulator
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);
    sim.exec(argc-n, argv+n);
    RSIM_Process *process = sim.get_process();
    RSIM_Thread *main_thread = process->get_main_thread();

    // Find the trigger address
    if (0==trigger_va) {
        if (trigger_func.empty() || !trigger_func.compare("oep")) {
            trigger_va = process->get_ep_orig_va();
        } else if (0==(trigger_va = RSIM_Tools::FunctionFinder().address(process->get_loads(), trigger_func))) {
            std::cerr <<argv[0] <<": unable to locate address of function: " <<trigger_func <<"\n";
            exit(1);
        }
    }
    
    // Install our disassembler callback to the main thread.  We don't use RSIM_Tools::MemoryDisassembler because we want to
    // cancel the specimen once we disassemble.
    main_thread->install_callback(new MyDisassembler(trigger_va, org));

    // Allow the specimen to run until the disassembly is triggered
    bool disassembled = false;
    sim.activate();
    try {
        sim.main_loop();
    } catch (MyDisassembler*) {
        disassembled = true;
    }
    if (!disassembled) {
        std::cerr <<argv[0] <<": specimen ran to completion without triggering a disassembly.\n";
        exit(1);
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
