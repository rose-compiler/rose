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


    /* Disassemble when we hit main() */
    rose_addr_t disassemble_va = RSIM_Tools::FunctionFinder().address(project, "main");
    assert(disassemble_va>0);
    sim.install_callback(new RSIM_Tools::MemoryDisassembler(disassemble_va));

    /* Print a stack trace and memory dump for every system call */
    struct SyscallStackTrace: public RSIM_Callbacks::SyscallCallback {
        virtual SyscallStackTrace *clone() { return this; }
        virtual bool operator()(bool enabled, const Args &args) {
            if (enabled) {
                RTS_Message *trace = args.thread->tracing(TRACE_SYSCALL);
                args.thread->report_stack_frames(trace, "Stack frames for following system call:");
                args.thread->get_process()->mem_showmap(trace, "Memory map for following system call:", "    ");
            }
            return enabled;
        }
    };
    sim.get_callbacks().add_syscall_callback(RSIM_Callbacks::BEFORE, new SyscallStackTrace);
    

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
