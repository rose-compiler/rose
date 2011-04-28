/* Emulates an executable. */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "RSIM_Adapter.h"

static RTS_mutex_t global_mutex = RTS_MUTEX_INITIALIZER(RTS_LAYER_RSIM_SIMULATOR_CLASS);
static bool do_disassemble_at_coredump = false;         /* disassemble when specimen is about to dump core? */
static std::set<rose_addr_t> do_disassemble_at_addr;    /* disassemble first time these instructions are hit. */
static bool do_show_disassembly = false;                /* show assembly code whenever we disassemble? */

/* Instruction callback to run the disassembler the first time we hit disassemble_at.  Once we hit it, we reset the
 * do_disassemble global and remove the callback from the thread and process. */
class DisassembleAtAddress: public RSIM_Callbacks::InsnCallback {
public:
    virtual DisassembleAtAddress *clone() { return this; }
    virtual bool operator()(bool prev, const Args &args) {
        RSIM_Process *process = args.thread->get_process();

        bool dis=false, clean=false;
        RTS_MUTEX(global_mutex) {
            if (do_disassemble_at_addr.empty()) {
                clean = true;
            } else if (do_disassemble_at_addr.find(args.insn->get_address())!=do_disassemble_at_addr.end()) {
                do_disassemble_at_addr.erase(args.insn->get_address());
                dis = true;
                if (do_disassemble_at_addr.empty())
                    clean = true;
            }
        } RTS_MUTEX_END;

        if (dis) {
            args.thread->tracing(TRACE_MISC)->mesg("disassembling at 0x%08"PRIx64"...\n", args.insn->get_address());
            SgAsmBlock *block = process->disassemble();
            if (do_show_disassembly)
                AsmUnparser().unparse(std::cout, block);
        }

        if (clean) {
            args.thread->get_callbacks().remove_insn_callback(RSIM_Callbacks::BEFORE, this);
            process->get_callbacks().remove_insn_callback(RSIM_Callbacks::BEFORE, this);
        }

        return prev;
    }
};

/* Process callback to run the disassembler when we're about to dump core. */
class DisassembleAtCoreDump: public RSIM_Callbacks::ProcessCallback {
public:
    virtual DisassembleAtCoreDump *clone() { return this; }
    virtual bool operator()(bool prev, const Args &args) {
        if (args.reason==COREDUMP) {
            RTS_Message tracer(args.process->get_tracing_file(), NULL);
            fprintf(stderr, "disassembling at core dump\n");
            args.process->mem_showmap(&tracer);
            SgAsmBlock *block = args.process->disassemble();
            if (do_show_disassembly)
                AsmUnparser().unparse(std::cout, block);
        }
        return prev;
    }
};

int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;

    /* Suck out any command-line switches that the tool recognizes.  Leave the others for processing by the RSIM_Simulator
     * class. */
    bool do_disassemble_at_oep;
    for (int i=1; i<argc; i++) {
        bool parsed = false;
        if (!strncmp(argv[i], "--disassemble=", 14)) {
            parsed = true;
            char *at = argv[i]+14;
            while (*at) {
                if (!strncmp(at, "oep", 3)) {
                    do_disassemble_at_oep = true;
                    at += 3;
                } else if (!strncmp(at, "core", 4)) {
                    do_disassemble_at_coredump = true;
                    at += 4;
                } else if (!strncmp(at, "show", 4)) {
                    do_show_disassembly = true;
                    at += 4;
                } else {
                    char *rest;
                    rose_addr_t addr = strtoull(at, &rest, 0);
                    if (*rest && *rest!=',') {
                        fprintf(stderr, "invalid argument for --disassemble switch: %s\n", at);
                        exit(1);
                    }
                    do_disassemble_at_addr.insert(addr);
                    at = rest;
                }
                if (','==*at)
                    at++;
            }
        }
        if (parsed) {
            memmove(argv+i, argv+i+1, (argc-i)*sizeof(*argv));
            --argc;
            --i;
        }
    }
    
    if (do_disassemble_at_oep || !do_disassemble_at_addr.empty())
        sim.get_callbacks().add_insn_callback(RSIM_Callbacks::BEFORE, new DisassembleAtAddress);
    if (do_disassemble_at_coredump)
        sim.get_callbacks().add_process_callback(RSIM_Callbacks::BEFORE, new DisassembleAtCoreDump);
                
        
            




    /***************************************************************************************************************************/
#   if 0 /* EXAMPLE: If you change this then also update the example text in RSIM_Callbacks.h */
    {
        /* This example depends on the previous one, which called RSIM_Process::disassemble() */
        class ShowFunction: public RSIM_Callbacks::InsnCallback {
        public:
            // Share a single callback among the simulator, the process, and all threads.
            virtual ShowFunction *clone() { return this; }

            // The actual callback.
            virtual bool operator()(bool prev, const Args &args) {
                SgAsmBlock *basic_block = isSgAsmBlock(args.insn->get_parent());
                SgAsmFunctionDeclaration *func = basic_block ? isSgAsmFunctionDeclaration(basic_block->get_parent()) : NULL;
                if (func && func->get_name()!=name) {
                    name = func->get_name();
                    args.thread->tracing(TRACE_MISC)->mesg("in function \"%s\"", name.c_str());
                }
                return prev;
            }

        private:
            std::string name;
        };

        sim.get_callbacks().add_insn_callback(RSIM_Callbacks::BEFORE, new ShowFunction);
    }
#   endif

    /***************************************************************************************************************************/
#   if 0 /*EXAMPLE*/
    {
        /* Shows how to replace a system call implementation so something else happens instead.  For instance, we replace the
         * open system call (#5) to ignore the file name and always open "/dev/null".  The system call tracing facility will
         * still report the original file name--we could supply an entry callback also if we wanted different behavior. */
        class NullOpen: public RSIM_Simulator::SystemCall::Callback {
        public:
            bool operator()(bool b, const Args &args) {
                uint32_t flags = args.thread->syscall_arg(1); // second argument
                uint32_t mode  = args.thread->syscall_arg(2); // third argument
                int fd = open("/dev/null", flags, mode);
                args.thread->syscall_return(fd);
                return b;
            }
        };

        sim.syscall_implementation(5)->body.clear().append(new NullOpen);
    }
#   endif

    /***************************************************************************************************************************/
#   if 0 /*EXAMPLE: Tracing file I/O */
    {
        RSIM_Adapter::TraceFileIO *tracer = new RSIM_Adapter::TraceFileIO;
        tracer->trace_fd(0); // stdin
        tracer->trace_fd(1); // stdout
        tracer->trace_fd(2); // stderr
        tracer->attach(&sim); //sim->adapt(tracer);
    }
#   endif

    /***************************************************************************************************************************/
#   if 0 /* EXAMPLE: disabling network capability */
    {
        static RSIM_Adapter::SyscallDisabler no_network(true);
        no_network.prefix("NoNetwork: ");
        no_network.disable_syscall(102/*sys_socketcall*/);
        no_network.attach(&sim);
    }
#   endif

    /***************************************************************************************************************************/
#   if 0 /* EXAMPLE: disabling system calls */
    {
        struct Ask: public RSIM_Simulator::SystemCall::Callback {
            std::set<int> asked;
            bool operator()(bool syscall_enabled, const Args &args) {
                if (asked.find(args.callno)==asked.end()) {
                    asked.insert(args.callno);
                    fprintf(stderr, "System call %d is currently disabled. What should I do? (s=skip, e=enable) [s] ",
                            args.callno);
                    char buf[200];
                    if (fgets(buf, sizeof buf, stdin) && buf[0]=='e')
                        syscall_enabled = true;
                }
                return syscall_enabled;
            }
        };

        static RSIM_Adapter::SyscallDisabler disabler(false);
        disabler.get_callbacks().append(new Ask);
        disabler.attach(&sim);
    }
#   endif
        
    /***************************************************************************************************************************/
#   if 0 /* EXAMPLE: Data injection */
#   endif








    /***************************************************************************************************************************
     *                                  The main program...
     ***************************************************************************************************************************/
    
    /* Configure the simulator by parsing command-line switches. The return value is the index of the executable name in argv. */
    int n = sim.configure(argc, argv, envp);

    /* Create the initial process object by loading a program and initializing the stack.   This also creates the main thread,
     * but does not start executing it. */
    sim.exec(argc-n, argv+n);
    if (do_disassemble_at_oep)
        do_disassemble_at_addr.insert(sim.get_process()->get_ep_orig_va());

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
