/* Robb's debugging area */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "RSIM_Adapter.h"


int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);

    /* Parse the ELF container so we can get to the symbol table. */
    char *rose_argv[4];
    rose_argv[0] = argv[0];
    rose_argv[1] = strdup("-rose:read_executable_file_format_only");
    rose_argv[2] = argv[n];
    rose_argv[3] = NULL;
    SgProject *project = frontend(3, rose_argv);

    /**************************************************************************************************************************
     *                                  Debugging callbacks...
     **************************************************************************************************************************/


#if 0
    /* Do we ever write to mem[gs-0x28=080c87d8]?  The native execution writes 0x080c5f00 to this address, but the simulation
     * for some reason doesn't. */
    sim.install_callback(new RSIM_Tools::MemoryAccessWatcher(0x080c87d8, 4));


    /* Disassemble when we hit memcpy() the first time, and also show stack traces */
    rose_addr_t memcpy_va = RSIM_Tools::FunctionFinder().address(project, "memcpy");
    std::cerr <<"memcpy() at " <<StringUtility::addrToString(memcpy_va) <<"\n";
    sim.install_callback(new RSIM_Tools::MemoryDisassembler(memcpy_va));
    sim.install_callback(new RSIM_Tools::FunctionReporter(true));


    /* Show memcpy arguments */
    struct MemcpyCall: public RSIM_Callbacks::InsnCallback {
        uint32_t memcpy_va;
        MemcpyCall(uint32_t memcpy_va): memcpy_va(memcpy_va) {}
        virtual MemcpyCall *clone() { return this; }
        virtual bool operator()(bool enabled, const Args &args) {
            if (!enabled) return false;
            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            if (args.insn->get_address()==memcpy_va) {
                /* Show arguments */
                uint32_t sp = args.thread->policy.readGPR(x86_gpr_sp).known_value();
                uint32_t actual[3];
                args.thread->get_process()->mem_read(actual, sp+4, 12);
                m->mesg("MemcpyCall triggered: entering memcpy(0x%08"PRIx32", 0x%08"PRIx32", 0x%08"PRIx32")",
                        actual[0], actual[1], actual[2]);
                args.thread->report_stack_frames(m, "", true);
            }
            return true;
        }
    };
    sim.install_callback(new MemcpyCall(memcpy_va));
#endif
    

    /***************************************************************************************************************************
     *                                  The main program...
     ***************************************************************************************************************************/
    
    sim.exec(argc-n, argv+n);
//    sim.activate();
    sim.main_loop();
//     sim.deactivate();
    sim.describe_termination(stderr);
    sim.terminate_self(); // probably doesn't return
    return 0;
}





#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
