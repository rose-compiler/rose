#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include <sys/wait.h>

RTS_rwlock_t RSIM_Simulator::class_rwlock = RTS_RWLOCK_INITIALIZER;
RSIM_Simulator *RSIM_Simulator::active_sim = NULL;

/* Constructor helper: defined here so the constructor can be modified without changing the header file, and therefore without
 * requiring that the entire librose.so sources be recompiled. */
void
RSIM_Simulator::ctor()
{
}

int
RSIM_Simulator::configure(int argc, char **argv, char **envp)
{
    int argno = 1;
    while (argno<argc && '-'==argv[argno][0]) {
        if (!strcmp(argv[argno], "--")) {
            argno++;
            break;

        } else if (!strncmp(argv[argno], "--log=", 6)) {
            /* Save log file name pattern, extending it to an absolute name in case the specimen changes directories */
            if (argv[argno][6]=='/') {
                trace_file_name = argv[argno]+6;
            } else {
                char dirname[4096];
                char *dirname_p = getcwd(dirname, sizeof dirname);
                ROSE_ASSERT(dirname_p);
                trace_file_name = std::string(dirname) + "/" + (argv[argno]+6);
            }
            argno++;

        } else if (!strncmp(argv[argno], "--debug=", 8)) {
            trace_flags = 0;
            char *s = argv[argno]+8;
            while (s && *s) {
                char *comma = strchr(s, ',');
                std::string word(s, comma?comma-s:strlen(s));
                s = comma ? comma+1 : NULL;
                if (word=="all") {
                    trace_flags = TRACE_ALL;
                } else if (word=="insn") {
                    trace_flags |= TRACE_INSN;
                } else if (word=="state") {
                    trace_flags |= TRACE_STATE;
                } else if (word=="mem") {
                    trace_flags |= TRACE_MEM;
                } else if (word=="mmap") {
                    trace_flags |= TRACE_MMAP;
                } else if (word=="signal") {
                    trace_flags |= TRACE_SIGNAL;
                } else if (word=="syscall") {
                    trace_flags |= TRACE_SYSCALL;
                } else if (word=="loader") {
                    trace_flags |= TRACE_LOADER;
                } else if (word=="progress") {
                    trace_flags |= TRACE_PROGRESS;
                } else if (word=="thread") {
                    trace_flags |= TRACE_THREAD;
                } else {
                    fprintf(stderr, "%s: debug words must be from the set: "
                            "all, insn, state, mem, mmap, syscall, signal, loader, progress, thread\n",
                            argv[0]);
                    exit(1);
                }
            }
            argno++;

        } else if (!strcmp(argv[argno], "--debug")) {
            trace_flags = TRACE_INSN | TRACE_SYSCALL | TRACE_SIGNAL;
            argno++;

        } else if (!strncmp(argv[argno], "--core=", 7)) {
            core_flags = 0;
            for (char *s=argv[argno]+7; s && *s; /*void*/) {
                if (!strncmp(s, "elf", 3)) {
                    s += 3;
                    core_flags |= CORE_ELF;
                } else if (!strncmp(s, "rose", 4)) {
                    s += 4;
                    core_flags |= CORE_ROSE;
                } else {
                    fprintf(stderr, "%s: unknown core dump type for %s\n", argv[0], argv[argno]);
                }
                while (','==*s) s++;
            }
            argno++;

        } else if (!strncmp(argv[argno], "--interp=", 9)) {
            interp_name = argv[argno++]+9;

        } else if (!strncmp(argv[argno], "--vdso=", 7)) {
            vdso_paths.clear();
            for (char *s=argv[argno]+7; s && *s; /*void*/) {
                char *colon = strchr(s, ':');
                vdso_paths.push_back(std::string(s, colon?colon-s:strlen(s)));
                s = colon ? colon+1 : NULL;
            }
            argno++;

        } else if (!strcmp(argv[argno], "--showauxv")) {
            fprintf(stderr, "showing the auxiliary vector for x86sim:\n");
            argno++;
            struct auxv_t {
                unsigned long type;
                unsigned long val;
            };
            char **p = envp;
            if (!p) {
                fprintf(stderr, "cannot show auxp (no envp)\n");
            } else {
                while (*p++);
                for (auxv_t *auxvp=(auxv_t*)p; 1; auxvp++) {
                    switch (auxvp->type) {
                        case 0:  fprintf(stderr, "    0  AT_NULL         %lu\n", auxvp->val); break;
                        case 1:  fprintf(stderr, "    1  AT_IGNORE       %lu\n", auxvp->val); break;
                        case 2:  fprintf(stderr, "    2  AT_EXECFD       %lu\n", auxvp->val); break;
                        case 3:  fprintf(stderr, "    3  AT_PHDR         0x%lx\n", auxvp->val); break;
                        case 4:  fprintf(stderr, "    4  AT_PHENT        0x%lx\n", auxvp->val); break;
                        case 5:  fprintf(stderr, "    5  AT_PHNUM        %lu\n", auxvp->val); break;
                        case 6:  fprintf(stderr, "    6  AT_PAGESZ       %lu\n", auxvp->val); break;
                        case 7:  fprintf(stderr, "    7  AT_BASE         0x%lx\n", auxvp->val); break;
                        case 8:  fprintf(stderr, "    8  AT_FLAGS        0x%lx\n", auxvp->val); break;
                        case 9:  fprintf(stderr, "    9  AT_ENTRY        0x%lx\n", auxvp->val); break;
                        case 10: fprintf(stderr, "    10 AT_NOTELF       %lu\n", auxvp->val); break;
                        case 11: fprintf(stderr, "    11 AT_UID          %ld\n", auxvp->val); break;
                        case 12: fprintf(stderr, "    12 AT_EUID         %ld\n", auxvp->val); break;
                        case 13: fprintf(stderr, "    13 AT_GID          %ld\n", auxvp->val); break;
                        case 14: fprintf(stderr, "    14 AT_EGID         %ld\n", auxvp->val); break;
                        case 15: fprintf(stderr, "    15 AT_PLATFORM     0x%lx\n", auxvp->val); break;
                        case 16: fprintf(stderr, "    16 AT_HWCAP        0x%lx\n", auxvp->val); break;
                        case 17: fprintf(stderr, "    17 AT_CLKTCK       %lu\n", auxvp->val); break;
                        case 18: fprintf(stderr, "    18 AT_FPUCW        %lu\n", auxvp->val); break;
                        case 19: fprintf(stderr, "    19 AT_DCACHEBSIZE  %lu\n", auxvp->val); break;
                        case 20: fprintf(stderr, "    20 AT_ICACHEBSIZE  %lu\n", auxvp->val); break;
                        case 21: fprintf(stderr, "    21 AT_UCACHEBSIZE  %lu\n", auxvp->val); break;
                        case 22: fprintf(stderr, "    22 AT_IGNOREPPC    %lu\n", auxvp->val); break;
                        case 23: fprintf(stderr, "    23 AT_SECURE       %ld\n", auxvp->val); break;

                        case 32: fprintf(stderr, "    32 AT_SYSINFO      0x%lx\n", auxvp->val); break;
                        case 33: fprintf(stderr, "    33 AT_SYSINFO_PHDR 0x%lx\n", auxvp->val); break;
                        case 34: fprintf(stderr, "    34 AT_L1I_CACHESHAPE 0x%lx\n", auxvp->val); break;
                        case 35: fprintf(stderr, "    35 AT_L1D_CACHESHAPE 0x%lx\n", auxvp->val); break;
                        case 36: fprintf(stderr, "    36 AT_L2_CACHESHAPE  0x%lx\n", auxvp->val); break;
                        case 37: fprintf(stderr, "    37 AT_L3_CACHESHAPE  0x%lx\n", auxvp->val); break;

                        default: fprintf(stderr, "    %lu AT_(unknown)   0x%lx\n", auxvp->type, auxvp->val); break;
                    }
                    if (!auxvp->type)
                        break;
                }
            }

        } else if (!strncmp(argv[argno], "--trace=", 8)) {
            if (btrace_file)
                fclose(btrace_file);
            if (NULL==(btrace_file=fopen(argv[argno]+8, "wb"))) {
                fprintf(stderr, "%s: %s: %s\n", argv[0], argv[argno]+8, strerror(errno));
                exit(1);
            }
#ifdef X86SIM_BINARY_TRACE_UNBUFFERED
            setbuf(btrace_file, NULL);
#endif
            argno++;

        } else {
            fprintf(stderr, "usage: %s [SWITCHES] PROGRAM ARGUMENTS...\n", argv[0]);
            exit(1);
        }
    }
    return argno;
}

int
RSIM_Simulator::exec(int argc, char **argv)
{
    ROSE_ASSERT(NULL==process); /* "There can be only one!" (main process, that is) */

    process = new RSIM_Process;
    process->set_tracing(stderr, trace_flags);
    process->set_core_styles(core_flags);
    process->set_interpname(interp_name);
    process->vdso_paths = vdso_paths;

    process->set_trace_name(trace_file_name);
    process->open_trace_file();

    SgAsmGenericHeader *fhdr = process->load(argv[0]);
    entry_va = fhdr->get_base_va() + fhdr->get_entry_rva();

    RSIM_Thread *main_thread = process->get_thread(getpid());
    assert(main_thread!=NULL);
    process->initialize_stack(fhdr, argc, argv);

    process->binary_trace_start();

    if (process->tracing(TRACE_MMAP)) {
        fprintf(process->tracing(TRACE_MMAP), "memory map after program load:\n");
        process->get_memory()->dump(process->tracing(TRACE_MMAP), "  ");
    }
    if (main_thread->tracing(TRACE_STATE)) {
        main_thread->tracing(TRACE_STATE)->mesg("Initial state:\n");
        main_thread->policy.dump_registers(main_thread->tracing(TRACE_STATE)->get_file());
    }

    return 0;
}

RSIM_Process *
RSIM_Simulator::create_process()
{
    ROSE_ASSERT(NULL==process); /* "There can be only one!" (main process, that is) */

    process = new RSIM_Process;
    process->set_tracing(stderr, trace_flags);
    process->set_core_styles(core_flags);
    process->set_interpname(interp_name);
    process->vdso_paths = vdso_paths;

    process->set_trace_name(trace_file_name);
    process->open_trace_file();
    return process;
}

void
RSIM_Simulator::activate()
{
    RTS_WRITE(class_rwlock) {
        if (!active_sim) {
            ROSE_ASSERT(0==active);

            /* Make this simulator active before we install the signal handlers. This is necessary because the signal handlers
             * reference this without using a mutex lock.  On the other hand, increment the "active" counter at the end of the
             * function, which allows the signal handler to determine when the sigaction vector is not fully initialized. */
            active_sim = this;
            memset(signal_installed, 0, sizeof signal_installed); /* no handler installed yet */
            memset(signal_restore, 0, sizeof signal_restore); /* cleaned up only for debugging */

            /* Register the inter-process signal reception handler for signals that are typically sent from one process to
             * another.  This signal handler simply places the signal onto a process-wide queue. */
            struct sigaction sa;
            sa.sa_handler = signal_receiver;
            sigfillset(&sa.sa_mask);
            sa.sa_flags = SA_RESTART;
            for (int signo=1; signo<__SIGRTMIN; signo++) {
                switch (signo) {
                    case SIGFPE:
                    case SIGILL:
                    case SIGSEGV:
                    case SIGBUS:
                    case SIGABRT:
                    case SIGTRAP:
                    case SIGSYS:
                        break;
                    default:
                        signal_installed[signo] = -1 == sigaction(signo, &sa, signal_restore+signo) ? -errno : 1;
                        break;
                }
            }

            /* Register the wakeup signal handler. This handler's only purpose is to interrupt blocked system calls. */
            sa.sa_handler = signal_wakeup;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            int signo = RSIM_SignalHandling::SIG_WAKEUP;
            signal_installed[signo] = -1 == sigaction(signo, &sa, signal_restore+signo) ? -errno : 1;
        } else {
            ROSE_ASSERT(active_sim==this);
        }
        active++;
    } RTS_WRITE_END;
}

void
RSIM_Simulator::deactivate()
{
    RTS_WRITE(class_rwlock) {
        /* The "active_sim" and "active" data members are adjusted in the opposite order as activate() in order that the signal
         * handlers can detect the current state of activation without using thread synchronization. */
        ROSE_ASSERT(this==active_sim);
        ROSE_ASSERT(active>0);
        if (0==--active) {
            for (int i=1; i<=_NSIG; i++) {
                if (signal_installed[i]>0) {
                    int status = sigaction(i, signal_restore+i, NULL);
                    ROSE_ASSERT(status>=0);
                }
            }
            active_sim = NULL;
        }
    } RTS_WRITE_END;
}

bool
RSIM_Simulator::is_active() const
{
    bool retval;
    RTS_READ(class_rwlock) {
        retval = active!=0;
    } RTS_READ_END;
    return retval;
}

/* Class method */
RSIM_Simulator *
RSIM_Simulator::which_active() 
{
    RSIM_Simulator *retval = NULL;
    RTS_READ(class_rwlock) {
        retval = active_sim;
    } RTS_READ_END;
    return retval;
}

/* Class method. This is a signal handler -- do not use thread synchronization or functions that are not async signal safe. */
void
RSIM_Simulator::signal_receiver(int signo)
{
    /* In order for this signal handler to be installed, there must be an active simulator. This is because the activate()
     * method installs the signal handler and the deactivate() removes it.  The active_sim is set before the signal handler is
     * installed and reset after it is removed. */
    RSIM_Simulator *simulator = active_sim;
    assert(simulator!=NULL);
    RSIM_Process *process = simulator->get_process();
    assert(process!=NULL);

    process->signal_enqueue(signo);
}

/* Class method. This is a signal handler -- do not use thread synchronization or functions that are not async signal safe. */
void
RSIM_Simulator::signal_wakeup(int signo)
{
    /* void; side effect is to interrupt blocked system calls. */
}

int
RSIM_Simulator::main_loop()
{
    RSIM_Process *process = get_process();
    RSIM_Thread *thread = process->get_thread(getpid());
    assert(thread!=NULL);

    /* The simulator's main thread is executed by the calling thread because the simulator's main thread must be a thread group
     * leader. */
    thread->main();
    return process->get_termination_status();
}

void
RSIM_Simulator::describe_termination(FILE *f)
{
    RTS_Message m(f, NULL);
    RSIM_Process *process = get_process();
    if (process->has_terminated()) {
        int status = process->get_termination_status();
        if (WIFEXITED(status)) {
            m.mesg("specimen %d exited with status %d\n", getpid(), WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            m.multipart("", "specimen %d exited due to signal ", getpid());
            print_enum(&m, signal_names, WTERMSIG(status));
            m.more(" (%s)%s\n", strsignal(WTERMSIG(status)), WCOREDUMP(status)?" core dumped":"");
        } else if (WIFSTOPPED(status)) {
            m.multipart("", "specimen %d is stopped due to signal ", getpid());
            print_enum(&m, signal_names, WSTOPSIG(status));
            m.more(" (%s)\n", strsignal(WSTOPSIG(status)));
        } else {
            m.mesg("specimen %d has unknown termination status: 0x%08x\n", getpid(), status);
        }
    } else {
        m.mesg("specimen %d has not exited yet\n", getpid());
    }
}

void
RSIM_Simulator::terminate_self()
{
    RSIM_Process *process = get_process();
    if (!process->has_terminated())
        return;

    RTS_WRITE(class_rwlock) {
        int status = process->get_termination_status();
        if (WIFEXITED(status)) {
            exit(WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            struct sigaction sa, old;
            memset(&sa, 0, sizeof sa);
            sa.sa_handler = SIG_DFL;
            sigaction(WTERMSIG(status), &sa, &old);
            raise(WTERMSIG(status));
            sigaction(WTERMSIG(status), &old, NULL);
        } else if (WIFSTOPPED(status)) {
            struct sigaction sa, old;
            memset(&sa, 0, sizeof sa);
            sa.sa_handler = SIG_DFL;
            sigaction(WTERMSIG(status), &sa, &old);
            raise(WTERMSIG(status));
            sigaction(WTERMSIG(status), &old, NULL);
        }
    } RTS_WRITE_END;
}

#endif /* ROSE_ENABLE_SIMULATOR */
