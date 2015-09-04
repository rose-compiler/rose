#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Simulator.h"
#include "Diagnostics.h"

#include <sys/wait.h>

using namespace rose::Diagnostics;

SAWYER_THREAD_TRAITS::RecursiveMutex RSIM_Simulator::class_rwlock;
RSIM_Simulator *RSIM_Simulator::active_sim = NULL;
Sawyer::Message::Facility RSIM_Simulator::mlog;

/******************************************************************************************************************************
 *                                      Simulator system calls
 *
 * These are extra system calls provided by the simulator.
 ******************************************************************************************************************************/

/*------------------------------------- is_present ---------------------------------------------------------------------------*/
static void
syscall_RSIM_is_present_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("RSIM_is_present");
}

static void
syscall_RSIM_is_present(RSIM_Thread *t, int callno)
{
    t->syscall_return(0);
}

static void
syscall_RSIM_is_present_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().str("\n");
}

/*------------------------------------- message ------------------------------------------------------------------------------*/
static void
syscall_RSIM_message_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("RSIM_message").s();
}

static void
syscall_RSIM_message(RSIM_Thread *t, int callno)
{
    t->syscall_return(0);
}

static void
syscall_RSIM_message_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().str("\n");
}

/*------------------------------------- delay --------------------------------------------------------------------------------*/
static void
syscall_RSIM_delay_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("RSIM_delay").d();
}

static void
syscall_RSIM_delay(RSIM_Thread *t, int callno)
{
    timespec tv, rem;
    tv.tv_sec = t->syscall_arg(0);
    tv.tv_nsec = 0;

    int result;
    while (-1==(result=nanosleep(&tv, &rem)) && EINTR==errno)
        tv = rem;
    t->syscall_return(result);
}

static void
syscall_RSIM_delay_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().str("\n");
}

/*------------------------------------- transaction --------------------------------------------------------------------------*/
static Translate transaction_flags[] = { TE2(0, start), TE2(1, rollback), TE2(2, commit), T_END };

static void
syscall_RSIM_transaction_enter(RSIM_Thread *t, int callno)
{
    int cmd = t->syscall_arg(0);
    if (1==cmd) { // rollback
        if (32 == t->get_process()->wordSize()) {
            t->syscall_enter("RSIM_transaction").e(transaction_flags).P(sizeof(pt_regs_32), print_pt_regs_32);
        } else {
            ASSERT_require(t->get_process()->wordSize() == 64);
            t->syscall_enter("RSIM_transaction").e(transaction_flags).P(sizeof(pt_regs_64), print_pt_regs_64);
        }
    } else {
        t->syscall_enter("RSIM_transaction").e(transaction_flags).p();
    }
}

static void
syscall_RSIM_transaction(RSIM_Thread *t, int callno)
{
    int cmd = t->syscall_arg(0);
    uint32_t regs_va = t->syscall_arg(1);
    int result = -ENOSYS;
    RSIM_Process *proc = t->get_process();
    std::string transaction_name = "specimen-initiated transaction " + StringUtility::addrToString(regs_va);

    switch (cmd) {
        case 0: {        // start
            if (32 == t->get_process()->wordSize()) {
                pt_regs_32 regs_guest = t->get_regs().get_pt_regs_32();
                if (sizeof(regs_guest)!=proc->mem_write(&regs_guest, regs_va, sizeof regs_guest)) {
                    result = -EFAULT;
                } else {
                    result = proc->mem_transaction_start(transaction_name); // total number of transactions
                    assert(-1==result || result>0);
                }
            } else {
                FIXME("[Robb P. Matzke 2015-05-28]: 64-bit not supported yet");
            }
            break;
        }

        case 1: {       // rollback
            if (32 == t->get_process()->wordSize()) {
                pt_regs_32 regs_guest;
                if (sizeof(regs_guest)!=proc->mem_read(&regs_guest, regs_va, sizeof regs_guest)) {
                    result = -EFAULT;
                } else if (0>=(result = t->get_process()->mem_transaction_rollback(transaction_name))) {
                    // error; don't initialize registers; this syscall will return without doing anything
                    if (0==result)
                        result = -EINVAL; // no such transaction
                } else {
                    // success.  The syscall will return as if we had called transaction start, but with zero to distinguish it
                    // from transaction start.
                    result = 0;
                    t->init_regs(PtRegs(regs_guest));
                }
            } else {
                FIXME("[Robb P. Matzke 2015-05-28]: 64-bit not supported yet");
            }
            break;
        }

        case 2:
            // not implemented yet
            break;
    }
    t->syscall_return(result);
}

static void
syscall_RSIM_transaction_leave(RSIM_Thread *t, int callno)
{
    int cmd = t->syscall_arg(0);
    int result = t->syscall_arg(-1);
    if (0==cmd && 0!=result) { // start
        if (32 == t->get_process()->wordSize()) {
            t->syscall_leave().ret().arg(1).P(sizeof(pt_regs_32), print_pt_regs_32).str("\n");
        } else {
            t->syscall_leave().ret().arg(1).P(sizeof(pt_regs_64), print_pt_regs_64).str("\n");
        }
    } else {
        t->syscall_leave().ret().str("\n");
    }
    t->get_process()->mem_showmap(t->tracing(TRACE_MMAP), "  memory map after transaction syscall:\n");
}

/******************************************************************************************************************************
 *                                      Class methods
 ******************************************************************************************************************************/


/* Constructor helper: defined here so the constructor can be modified without changing the header file, and therefore without
 * requiring that the entire librose.so sources be recompiled. */
void
RSIM_Simulator::ctor()
{
    static bool classInitialized = false;
    if (!classInitialized) {
        rose::Diagnostics::initialize();
        mlog = Sawyer::Message::Facility("RSIM", rose::Diagnostics::destination);
        rose::Diagnostics::mfacilities.insertAndAdjust(mlog);
        classInitialized = true;
    }
    
    /* Some special syscalls that are available to specimens when they're being simulated. */
    syscall_define(1000000, syscall_RSIM_is_present_enter,  syscall_RSIM_is_present,  syscall_RSIM_is_present_leave);
    syscall_define(1000001, syscall_RSIM_message_enter,     syscall_RSIM_message,     syscall_RSIM_message_leave);
    syscall_define(1000002, syscall_RSIM_delay_enter,       syscall_RSIM_delay,       syscall_RSIM_delay_leave);
    syscall_define(1000003, syscall_RSIM_transaction_enter, syscall_RSIM_transaction, syscall_RSIM_transaction_leave);
}

void
RSIM_Simulator::configure(const Settings &providedSettings, char **envp) {
    settings_ = providedSettings;                       // copy them, then perhaps modify them

    // Use absolute name in case process changes directory
    if (!settings_.tracingFileName.empty() && '/'!=settings_.tracingFileName[0]) {
        char dirname[4096];
        char *dirname_p = getcwd(dirname, sizeof dirname);
        ASSERT_always_not_null(dirname_p);
        settings_.tracingFileName = std::string(dirname) + "/" + settings_.tracingFileName;
    }

    // Turn on some tracing
    tracingFlags_ = tracingFacilityBit(TRACE_MISC);
    BOOST_FOREACH (TracingFacility t, settings_.tracing)
        tracingFlags_ |= tracingFacilityBit(t);

    // Core dump styles
    core_flags = 0;
    BOOST_FOREACH (CoreStyle cs, settings_.coreStyles)
        core_flags |= cs;

    // Global semaphore.
    std::string semname = settings_.semaphoreName;
    if (!semname.empty() && '/'!=semname[0])
        semname = "/" + semname;
    if (semname.find_first_of('/', 1)!=std::string::npos) {
        std::cerr <<"invalid semaphore name \"" <<semname <<"\"; should not contain internal slashes\n";
        exit(1);
    }
    settings_.semaphoreName = semname;
    set_semaphore_name(semname);

    if (!settings_.binaryTraceName.empty()) {
        if (btrace_file)
            fclose(btrace_file);
        if (NULL==(btrace_file=fopen(settings_.binaryTraceName.c_str(), "wb"))) {
            std::cerr <<strerror(errno) <<": " <<settings_.binaryTraceName <<"\n";
            exit(1);
        }
#ifdef X86SIM_BINARY_TRACE_UNBUFFERED
        setbuf(btrace_file, NULL);
#endif
    }

    // Show auxv
    if (settings_.showAuxv) {
        fprintf(stderr, "showing the auxiliary vector for x86sim:\n");
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
    }
}

Sawyer::CommandLine::SwitchGroup
RSIM_Simulator::commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Simulator switches");

    sg.insert(Switch("tracefile")
              .argument("filename", anyParser(settings.tracingFileName))
              .doc("Name of tracing file. Any occurrance of the substring \"${pid}\" will be replaced with the "
                   "specimen process ID."));

    sg.insert(Switch("trace")
              .argument("how", listParser(enumParser<TracingFacility>(settings.tracing)
                                          ->with("all", TRACE_NFACILITIES)
                                          ->with("insn", TRACE_INSN)
                                          ->with("state", TRACE_STATE)
                                          ->with("mem", TRACE_MEM)
                                          ->with("mmap", TRACE_MMAP)
                                          ->with("signal", TRACE_SIGNAL)
                                          ->with("syscall", TRACE_SYSCALL)
                                          ->with("loader", TRACE_LOADER)
                                          ->with("progress", TRACE_PROGRESS)
                                          ->with("thread", TRACE_THREAD)
                                          ->with("futex", TRACE_FUTEX)))
              .whichValue(SAVE_ALL)
              .explosiveLists(true)
              .doc("Tracing categories. The following words can be specified either as part of a comma-separated list "
                   "or with separate instances of this switch: all, insn, state, mem, mmap, signal, syscall, loader, "
                   "progress, thread, futex."));

    sg.insert(Switch("core")
              .argument("how", listParser(enumParser<CoreStyle>(settings.coreStyles)
                                          ->with("elf", CORE_ELF)
                                          ->with("rose", CORE_ROSE)))
              .whichValue(SAVE_ALL)
              .explosiveLists(true)
              .doc("Controls which style of core dump is created. Values are \"elf\" or \"rose\"."));

    sg.insert(Switch("interpreter")
              .argument("name", anyParser(settings.interpreterName))
              .doc("Name of the interpreter. On Linux, this is the name of the dynamic linker."));

    sg.insert(Switch("vdso")
              .argument("name", listParser(anyParser(settings.vdsoPaths)))
              .whichValue(SAVE_ALL)
              .explosiveLists(true)
              .doc("The linux operating system maps a page of executable memory called the virtual dynamic shared "
                   "object (vdso) named \"linux-gate.so\" (32 bit) or \"linux-vdso.so\" (64 bit). This file does not "
                   "normally exist in the filesystem, but the simulator is able to load an arbitrary file as the VDSO. "
                   "This switch can provide the names of the directories to search for a file named either \"linux-gate.so\" "
                   "or \"linux-vdso.so\" (depending on the word size).  It can also be used to specify a particular file "
                   "name.  Multiple directories are specified either by separating them with comma, semicolon, or colon "
                   "or by using this switch multiple times."));

    sg.insert(Switch("vsyscall")
              .argument("name", listParser(anyParser(settings.vsyscallPaths)))
              .whichValue(SAVE_ALL)
              .explosiveLists(true)
              .doc("The linux-amd64 operating system maps a page of executable memory at 0xffffffffff600000 that "
                   "contains up to three system calls: time, gettimeofday, and getcpu. The purpose is to be able to "
                   "service these calls without the usual more expensive switch to kernel mode.  The simulator must "
                   "also map code at this address or else those system calls will segfault.  The simulator looks for "
                   "a file named \"vsyscall-amd64\" in various directories and maps the first such file found.  This "
                   "switch can be used to override the list of directoies that are searched, or can even provide the "
                   "name of a particular file.  The default search paths are the current working directory, the ROSE "
                   "build directory, the ROSE source directory, and the ROSE data installation directory.  Multiple "
                   "directories are specified either by separating them with comma, semicolon, or colon or by using "
                   "this switch multiple times."));

    sg.insert(Switch("semaphore")
              .argument("name", anyParser(settings.semaphoreName))
              .doc("Name of the semaphore for inter-process synchronization."));

    sg.insert(Switch("show-auxv")
              .intrinsicValue(true, settings.showAuxv)
              .doc("Show the auxiliary vector for this tool."));

    sg.insert(Switch("binary-trace")
              .argument("filename", anyParser(settings.binaryTraceName))
              .doc("Name of file for binary tracing."));

    sg.insert(Switch("native-load")
              .intrinsicValue(true, settings.nativeLoad)
              .doc("If the executable format allows it to be run natively, then a debugger can be used to initialize the "
                   "simulation machine state. Setting up the simulated machine state to exactly match the state produced "
                   "by the linux kernel on a particular architecture and machine is complex and prone to slight differences. "
                   "By using a debugger, the Linux kernel does the loading and initialization in a native process, "
                   "then ROSE copies that process's memory and register state into ROSE's simulation, and finally the "
                   "native process is destroyed without it ever executing any instructions. The @s{no-native-load} causes "
                   "ROSE to simulate all kernel actions internally.  The default is to " +
                   std::string(settings.nativeLoad?"load natively.":"emulate the load in ROSE.")));
    sg.insert(Switch("no-native-load")
              .key("native-load")
              .intrinsicValue(false, settings.nativeLoad)
              .hidden(true));

    return sg;
}

// Construct the AST for the main executable without further linking, mapping
SgAsmInterpretation *
RSIM_Simulator::parseMainExecutable(RSIM_Process *process) {
    char *frontend_args[4];
    frontend_args[0] = strdup("-");
    frontend_args[1] = strdup("-rose:read_executable_file_format_only"); /*delay disassembly until later*/
    frontend_args[2] = strdup(exeArgs()[0].c_str());
    frontend_args[3] = NULL;
    SgProject *project = frontend(3, frontend_args);

    // Find the best interpretation and file header.  Windows PE programs have two where the first is DOS and the second is PE
    // (we'll use the PE interpretation).
    return SageInterface::querySubTree<SgAsmInterpretation>(project).back();
}

int
RSIM_Simulator::loadSpecimen(pid_t existingPid) {
    char cmd[8192];
    ssize_t nread = readlink(("/proc/" + StringUtility::numberToString(existingPid) + "/exe").c_str(), cmd, sizeof cmd);
    if (-1 == nread)
        return -errno;
    if ((size_t)nread + 1 >= sizeof cmd)
        return -ENAMETOOLONG;
    return loadSpecimen(std::vector<std::string>(1, cmd), existingPid);
}

int
RSIM_Simulator::loadSpecimen(const std::vector<std::string> &args, int existingPid/*=-1*/)
{
    ASSERT_require2(exeArgs_.empty(), "specimen cannot be loaded twice");
    ASSERT_forbid2(args.empty(), "we must at least have an executable name");

    // Save arguments
    exeName_ = args[0];
    exeArgs_ = args;

    create_process();

    if (int error = process->load(existingPid))
        return error;
    SgAsmGenericHeader *fhdr = process->mainHeader();
    entry_va = process->entryPointOriginalVa();

    RSIM_Thread *mainThread = process->get_main_thread();
    initializeStackArch(mainThread, fhdr);

    process->binary_trace_start();

    if ((process->tracingFlags() & tracingFacilityBit(TRACE_MMAP))) {
        fprintf(process->tracingFile(), "memory map after program load:\n");
        process->get_memory().dump(process->tracingFile(), "  ");
    }

    mainThread->tracing(TRACE_STATE) <<"Initial state:\n"
                                     <<*mainThread->operators()->get_state()->get_register_state();

    return 0;
}

RSIM_Process *
RSIM_Simulator::create_process()
{
    ROSE_ASSERT(NULL==process); /* "There can be only one!" (main process, that is) */

    process = new RSIM_Process(this);
    process->set_callbacks(callbacks);
    process->set_tracing(stderr, tracingFlags_);
    process->set_core_styles(core_flags);
    process->set_interpname(settings_.interpreterName);

    process->tracingName(settings_.tracingFileName);
    process->open_tracing_file();
    return process;
}

void
RSIM_Simulator::activate()
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(class_rwlock);
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
        sa.sa_flags = SA_RESTART | SA_SIGINFO;
        sa.sa_handler = NULL;
        sa.sa_sigaction = signal_receiver;
        sigfillset(&sa.sa_mask);
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
}

void
RSIM_Simulator::deactivate()
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(class_rwlock);

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
}

bool
RSIM_Simulator::is_active() const
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(class_rwlock);
    return active!=0;
}

/* Class method */
RSIM_Simulator *
RSIM_Simulator::which_active() 
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(class_rwlock);
    return active_sim;
}

/* Class method. This is a signal handler -- do not use thread synchronization or functions that are not async signal safe. */
void
RSIM_Simulator::signal_receiver(int signo, siginfo_t *info, void*)
{
    /* In order for this signal handler to be installed, there must be an active simulator. This is because the activate()
     * method installs the signal handler and the deactivate() removes it.  The active_sim is set before the signal handler is
     * installed and reset after it is removed. */
    RSIM_Simulator *simulator = active_sim;
    assert(simulator!=NULL);
    RSIM_Process *process = simulator->get_process();
    assert(process!=NULL);

#if 1 /* WARNING: this is not async signal safe, but useful for debugging */
    char buf[1024];
    sprintf(buf, "PID %d received signal %d with info=%p\n", getpid(), signo, info);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_signo = %d\n", info->si_signo);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_errno = %d\n", info->si_errno);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_code  = %d\n", info->si_code);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_pid   = %d\n", info->si_pid);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_uid   = %u\n", info->si_uid);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_int   = %u\n", info->si_int);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_ptr   = %p\n", info->si_ptr);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_status = %u\n", info->si_status);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_utime = %ld\n", info->si_utime);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_stime = %ld\n", info->si_stime);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_addr  = %p\n", info->si_addr);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_band  = %ld\n", info->si_band);
    write(2, buf, strlen(buf));
    sprintf(buf, "    info.si_fd    = %d\n", info->si_fd);
    write(2, buf, strlen(buf));
#endif

    process->signal_enqueue(RSIM_SignalHandling::mk(info));
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
    RSIM_Thread *thread = process->get_main_thread();

    /* The simulator's main thread is executed by the calling thread because the simulator's main thread must be a thread group
     * leader. */
    bool cb_process_status = process->get_callbacks().call_process_callbacks(RSIM_Callbacks::BEFORE, process,
                                                                             RSIM_Callbacks::ProcessCallback::START,
                                                                             true);

    // The process' main thread has already been created and initialized but has not started running yet.
    thread->start();
    thread->waitForState(RSIM_Thread::TERMINATED);

    process->get_callbacks().call_process_callbacks(RSIM_Callbacks::AFTER, process,
                                                    RSIM_Callbacks::ProcessCallback::FINISH,
                                                    cb_process_status);

    return process->get_termination_status();
}

std::string
RSIM_Simulator::describe_termination()
{
    std::ostringstream m;
    RSIM_Process *process = get_process();
    if (process->has_terminated()) {
        int status = process->get_termination_status();
        if (WIFEXITED(status)) {
            mfprintf(m)("specimen %d exited with status %d", getpid(), WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            mfprintf(m)("specimen %d exited due to signal %s %s",
                        getpid(), strsignal(WTERMSIG(status)), WCOREDUMP(status)?" core dumped":"");
        } else if (WIFSTOPPED(status)) {
            mfprintf(m)("specimen %d is stopped due to signal %s", getpid(), strsignal(WSTOPSIG(status)));
        } else {
            mfprintf(m)("specimen %d has unknown termination status: 0x%08x", getpid(), status);
        }
    } else {
        mfprintf(m)("specimen %d has not exited yet", getpid());
    }
    return m.str();
}

void
RSIM_Simulator::terminate_self()
{
    RSIM_Process *process = get_process();
    if (!process->has_terminated())
        return;

    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(class_rwlock);
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
}

bool
RSIM_Simulator::syscall_is_implemented(int callno) const
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(instance_rwlock);
    std::map<int, SystemCall*>::const_iterator found = syscall_table.find(callno);
    bool retval = (found!=syscall_table.end() &&
                   (!found->second->enter.empty() || !found->second->body.empty() || !found->second->leave.empty()));
    return retval;
}

RSIM_Simulator::SystemCall *
RSIM_Simulator::syscall_implementation(int callno)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(instance_rwlock);
    SystemCall *retval = NULL;
    std::map<int, SystemCall*>::iterator found = syscall_table.find(callno);
    if (found==syscall_table.end()) {
        retval = syscall_table[callno] = new SystemCall;
    } else {
        retval = found->second;
    }
    return retval;
}

void
RSIM_Simulator::syscall_define(int callno,
                               void(*enter)(RSIM_Thread*, int callno),
                               void(*body)(RSIM_Thread*, int callno),
                               void(*leave)(RSIM_Thread*, int callno))
{
    if (enter)
        syscall_implementation(callno)->enter.append(new SystemCall::Function(enter));
    if (body)
        syscall_implementation(callno)->body .append(new SystemCall::Function(body ));
    if (leave)
        syscall_implementation(callno)->leave.append(new SystemCall::Function(leave));
}

bool
RSIM_Simulator::set_semaphore_name(const std::string &name, bool do_unlink/*=false*/)
{
    if (global_semaphore)
        return false; // can't change the name if we've already created the semaphore
    global_semaphore_name = name;
    global_semaphore_unlink = do_unlink;
    return true;
}

const std::string &
RSIM_Simulator::get_semaphore_name() const
{
    return global_semaphore_name;
}

sem_t *
RSIM_Simulator::get_semaphore(bool *unlinked/*=NULL*/)
{
    if (NULL==global_semaphore) {
        if (global_semaphore_name.empty()) {
            global_semaphore_name = "/ROSE_simulator-pid=" + StringUtility::numberToString(getpid());
            global_semaphore_unlink = true;
        }
        fprintf(stderr, "RSIM_Simulator::get_semaphore() is using %s\n", global_semaphore_name.c_str());
        global_semaphore = sem_open(global_semaphore_name.c_str(), O_CREAT, 0666, 1);
        assert(SEM_FAILED!=global_semaphore);
        if (global_semaphore_unlink)
            sem_unlink(global_semaphore_name.c_str()); /* unlink now, destroyed when all closed (at simulator process exit) */
    }
    if (unlinked)
        *unlinked = global_semaphore_unlink;
    return global_semaphore;
}

/* Install callback in the simulator and optionally in the existing process and threads. */
#define RSIM_SIMULATOR_DEFINE_INSTALL_CALLBACK(CALLBACK_CLASS, INSERTION_FUNCTION)                                             \
    void                                                                                                                       \
    RSIM_Simulator::install_callback(CALLBACK_CLASS *cb, RSIM_Callbacks::When when, bool everywhere)                           \
    {                                                                                                                          \
        if (cb) {                                                                                                              \
            callbacks.INSERTION_FUNCTION(when, cb);                                                                            \
            if (everywhere && get_process())                                                                                   \
                get_process()->install_callback(dynamic_cast<CALLBACK_CLASS*>(cb->clone()), when, everywhere);                 \
        }                                                                                                                      \
    }
RSIM_SIMULATOR_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::InsnCallback,    add_insn_callback);
RSIM_SIMULATOR_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::MemoryCallback,  add_memory_callback);
RSIM_SIMULATOR_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::SyscallCallback, add_syscall_callback);
RSIM_SIMULATOR_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::SignalCallback,  add_signal_callback);
RSIM_SIMULATOR_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::ThreadCallback,  add_thread_callback);
RSIM_SIMULATOR_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::ProcessCallback, add_process_callback);
#undef RSIM_SIMULATOR_DEFINE_INSTALL_CALLBACK

/* Remove callback in the simulator and optionally in the existing process and threads. */
#define RSIM_SIMULATOR_DEFINE_REMOVE_CALLBACK(CALLBACK_CLASS, REMOVAL_FUNCTION)                                                \
    void                                                                                                                       \
    RSIM_Simulator::remove_callback(CALLBACK_CLASS *cb, RSIM_Callbacks::When when, bool everywhere)                            \
    {                                                                                                                          \
        if (cb) {                                                                                                              \
            callbacks.REMOVAL_FUNCTION(when, cb);                                                                              \
            if (everywhere && get_process())                                                                                   \
                get_process()->remove_callback(cb, when, everywhere);                                                          \
        }                                                                                                                      \
    }
RSIM_SIMULATOR_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::InsnCallback,    remove_insn_callback);
RSIM_SIMULATOR_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::MemoryCallback,  remove_memory_callback);
RSIM_SIMULATOR_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::SyscallCallback, remove_syscall_callback);
RSIM_SIMULATOR_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::SignalCallback,  remove_signal_callback);
RSIM_SIMULATOR_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::ThreadCallback,  remove_thread_callback);
RSIM_SIMULATOR_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::ProcessCallback, remove_process_callback);
#undef RSIM_SIMULATOR_DEFINE_REMOVE_CALLBACK

        

#endif /* ROSE_ENABLE_SIMULATOR */
