#include "sage3basic.h"

#include "RSIM_Simulator.h"

#include <sys/wait.h>

RSIM_Simulator *RSIM_Simulator::active_sim;

void
RSIM_Simulator::ctor(int argc, char **argv, char **envp)
{
    int argno = parse_cmdline(argc, argv, envp);
    ROSE_ASSERT(argc-argno>=1); /* executable plus its arguments */

    RSIM_Process *process = create_process();
    SgAsmGenericHeader *fhdr = process->load(argv[argno]);      /* creates initial thread */
    entry_va = fhdr->get_base_va()+fhdr->get_entry_rva();

    RSIM_Thread *main_thread = process->get_thread(getpid());
    main_thread->initialize_stack(fhdr, argc-argno, argv+argno);

    process->binary_trace_start();

    if (process->tracing(TRACE_MMAP)) {
        fprintf(process->tracing(TRACE_MMAP), "memory map after program load:\n");
        process->get_memory()->dump(process->tracing(TRACE_MMAP), "  ");
    }
    if (main_thread->tracing(TRACE_STATE)) {
        fprintf(main_thread->tracing(TRACE_STATE), "Initial state:\n");
        main_thread->policy.dump_registers(main_thread->tracing(TRACE_STATE));
    }
}

int
RSIM_Simulator::parse_cmdline(int argc, char **argv, char **envp)
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
                } else {
                    fprintf(stderr, "%s: debug words must be from the set: "
                            "all, insn, state, mem, mmap, syscall, signal, loader, progress\n",
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

        } else if (!strncmp(argv[argno], "--dump=", 7)) {
            char *rest;
            errno = 0;
            dump_at = strtoul(argv[argno]+7, &rest, 0);
            if (rest==argv[argno]+7 || errno!=0) {
                fprintf(stderr, "%s: --dump=N requires an address, N\n", argv[0]);
                exit(1);
            }
            if (','==rest[0] && rest[1])
                dump_name = rest+1;
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
    ROSE_ASSERT(!which_active());               /* no simulator (including this one) should be active */
    
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    for (int i=1; i<=_NSIG; i++)
        signal_installed[i] = -1==sigaction(i, &sa, signal_restore+i) ? -errno : 0;

    active_sim = this;
    this->active = true;
}

void
RSIM_Simulator::deactivate()
{
    ROSE_ASSERT(this==which_active());
    for (int i=1; i<=_NSIG; i++) {
        if (signal_installed[i]>=0) {
            int status = sigaction(i, signal_restore+i, NULL);
            ROSE_ASSERT(status>=0);
        }
    }

    active_sim = NULL;
    this->active = false;
}

/* Class method */
void
RSIM_Simulator::signal_handler(int signo)
{
    RSIM_Simulator *simulator = which_active();
    if (simulator) {
        //FIXME: this sends all signals to the main thread
        simulator->get_process()->get_thread(getpid())->signal_generate(signo);
    } else {
        fprintf(stderr, "RSIM::Simulator::signal_handler(): received ");
        print_enum(stderr, signal_names, signo);
        fprintf(stderr, "(%d); this signal cannot be delivered so we're taking it ourselves\n", signo);
        struct sigaction sa, old_sa;
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sigaction(signo, &sa, &old_sa);
        sigset_t ss, old_ss;
        sigemptyset(&ss);
        sigaddset(&ss, signo);
        sigprocmask(SIG_UNBLOCK, &ss, &old_ss);
        raise(signo);
        sigprocmask(SIG_SETMASK, &old_ss, NULL);
        sigaction(signo, &old_sa, NULL);
    }
}

void
RSIM_Simulator::main_loop()
{
    RSIM_Process *process = get_process();
    RSIM_Thread *thread = process->get_thread(getpid());
    assert(thread!=NULL);

    while (true) {
        thread->report_progress_maybe();
        try {
            if (thread->policy.readIP().known_value()==thread->SIGHANDLER_RETURN) {
                thread->signal_return();
                if (dump_at!=0 && dump_at==thread->SIGHANDLER_RETURN) {
                    fprintf(stderr, "Reached dump point.\n");
                    process->dump_core(SIGABRT, dump_name);
                    dump_at = 0;
                }
                continue;
            }

            if (dump_at!=0 && dump_at == thread->policy.readIP().known_value()) {
                fprintf(stderr, "Reached dump point.\n");
                process->dump_core(SIGABRT, dump_name);
                dump_at = 0;
            }

            SgAsmx86Instruction *insn = thread->current_insn();
            if (!seen_entry_va && insn->get_address()==entry_va) {
                seen_entry_va = true;
                if (thread->tracing(TRACE_MMAP)) {
                    fprintf(thread->tracing(TRACE_MMAP), "memory map at program entry:\n");
                    process->get_memory()->dump(thread->tracing(TRACE_MMAP), "  ");
                }
            }
            process->binary_trace_add(thread, insn);
            thread->semantics.processInstruction(insn);
            if (thread->tracing(TRACE_STATE))
                thread->policy.dump_registers(thread->tracing(TRACE_STATE));

            thread->signal_deliver_any();
        } catch (const RSIM_Semantics::Exception &e) {
            /* Thrown for instructions whose semantics are not implemented yet. */
            std::cerr <<e <<"\n\n";
#ifdef X86SIM_STRICT_EMULATION
            process->dump_core(SIGILL);
            abort();
#else
            std::cerr <<"Ignored. Continuing with a corrupt state...\n";
#endif
        } catch (const RSIM_SEMANTIC_POLICY::Exception &e) {
            std::cerr <<e <<"\n\n";
            process->dump_core(SIGILL);
            abort();
        } catch (const RSIM_Thread::Exit &e) {
            /* specimen has exited */
            if (thread->robust_list_head_va)
                fprintf(stderr, "warning: robust_list not cleaned up\n"); /* FIXME: see set_robust_list() syscall */
            if (WIFEXITED(e.status)) {
                fprintf(stderr, "specimen exited with status %d\n", WEXITSTATUS(e.status));
		if (WEXITSTATUS(e.status))
                    exit(WEXITSTATUS(e.status));
            } else if (WIFSIGNALED(e.status)) {
                fprintf(stderr, "specimen exited due to signal ");
                print_enum(stderr, signal_names, WTERMSIG(e.status));
                fprintf(stderr, " (%s)%s\n", strsignal(WTERMSIG(e.status)), WCOREDUMP(e.status)?" core dumped":"");
                exit(1);
            } else if (WIFSTOPPED(e.status)) {
                fprintf(stderr, "specimen is stopped due to signal ");
                print_enum(stderr, signal_names, WSTOPSIG(e.status));
                fprintf(stderr, " (%s)\n", strsignal(WSTOPSIG(e.status)));
                exit(1);
            }
            break;
        } catch (const RSIM_SemanticPolicy::Signal &e) {
            thread->signal_generate(e.signo);
        }
    }
}

            
