/* Emulates an executable. */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"

class MySim: public RSIM_Linux32 {
public:
#if 0 /*EXAMPLE*/
    /* Shows how to replace a system call implementation so something else happens instead.  For instance, we replace the
     * open system call to ignore the file name and always open "/dev/null".  The system call tracing facility will still
     * report the original file name--we could supply an entry callback also if we wanted different behavior. */
    MySim() {
        // 5 == open; see <asm/unistd_32.h>
        SystemCall sc = syscall_get(5);
        sc.body = null_open;
        syscall_set(5, sc);
    }
    static void null_open(RSIM_Thread *t, int callno) {
        uint32_t flags = t->syscall_arg(1);
        uint32_t mode  = (flags & O_CREAT) ? t->syscall_arg(2) : 0;
        int fd = open("/dev/null", flags, mode);
        t->syscall_return(fd);
    }
#endif
};
        

int
main(int argc, char *argv[], char *envp[])
{
    MySim sim;

#if 1 /*EXAMPLE: If you change this, then also update the example text in RSIM_Callbacks.h. */
    {
        /* An example of a pre-instruction callback which disassembles the specimen's memory image when a thread attempts to
         * execute at the original entry point (OEP) for the first time.  The OEP is the entry address defined in the ELF file
         * header for the specimen's executable, which is reached after the dynamic linker is simulated and thus after all
         * required dynamic libraries have been resolved and are in memory.  A single callback object is shared between the
         * simulator, the process, and all threads by virtue of the clone() method being a no-op.  This callback is removed
         * from the calling thread's list of pre-instruction callbacks the first time it's called, but it would still be
         * invoked if any other thread reaches the EOP (this doesn't normally happen in practice).  We also remove it from the
         * calling thread's process callbacks so that new threads will not have the per-instruction overhead (in practice,
         * there is only one thread when the dynamic linker branches to the OEP). */
        struct DisassembleAtOep: public RSIM_Callbacks::InsnCallback {
            virtual DisassembleAtOep *clone() { return this; }
            virtual bool operator()(bool prev, const Args &args) {
                RSIM_Process *process = args.thread->get_process();
                if (process->get_ep_orig_va() == args.insn->get_address()) {
                    std::cout <<"disassembling at OEP...\n";
                    SgAsmBlock *block = process->disassemble();
                    AsmUnparser().unparse(std::cout, block);
                    args.thread->get_callbacks().remove_insn_callback(RSIM_Callbacks::BEFORE, this);
                    process->get_callbacks().remove_insn_callback(RSIM_Callbacks::BEFORE, this);
                }
                return prev;
            }
        };
        sim.get_callbacks().add_insn_callback(RSIM_Callbacks::BEFORE, new DisassembleAtOep);
    }
#endif


    /* Configure the simulator by parsing command-line switches. The return value is the index of the executable name in argv. */
    int n = sim.configure(argc, argv, envp);

    /* Create the initial process object by loading a program and initializing the stack.   This also creates the main thread,
     * but does not start executing it. */
    sim.exec(argc-n, argv+n);

    /* Get ready to execute by making the specified simulator active. This sets up signal handlers, etc. */
    sim.activate();

    /* Allow executor threads to run and return when the simulated process terminates. The return value is the termination
     * status of the simulated program. */
    sim.main_loop();

    /* Not really necessary since we're not doing anything else. */
    sim.deactivate();

    /* Describe termination status, and then exit ourselves with that same status. */
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
