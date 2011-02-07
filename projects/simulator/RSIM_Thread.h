#ifndef ROSE_RSIM_Thread_H
#define ROSE_RSIM_Thread_H

class RSIM_Thread {
public:
    /* Thrown by exit system calls. */
    struct Exit {
        explicit Exit(int status): status(status) {}
        int status;                             /* same value as returned by waitpid() */
    };

public:
    RSIM_Thread(RSIM_Process *process)
        : process(process), policy(this), semantics(policy), disassembler(NULL), report_interval(10.0),
          robust_list_head_va(0), set_child_tid(0), clear_child_tid(0),
          signal_pending(0), signal_mask(0), signal_reprocess(false) {
    }
    
    /** Initialize the stack for the specimen.  The argc and argv are the command-line of the specimen, not ROSE or the
     *  simulator. */
    void initialize_stack(SgAsmGenericHeader*, int argc, char *argv[]);

    /** Return the file descriptor used for a debugging facility.  If the facility is disabled then a null pointer is returned. */
    FILE *tracing(unsigned what) const;

    /** Return the thread ID. Since each simulated thread is mapped to a unique real thread, the ID of the real thread also
     *  serves as the ID of the simulated thread. Linux uses one process per thread, so a thread ID is the same as a process
     *  ID. */
    int get_tid() {
        return getpid();
    }
    
    /** Print a progress report if progress reporting is enabled and enough time has elapsed since the previous report. */
    void report_progress_maybe();

    /** Recursively load an executable and its libraries libraries into memory, creating the MemoryMap object that describes
     *  the mapping from the specimen's address space to the simulator's address space.
     *
     *  There are two ways to load dynamic libraries:
     *  <ul>
     *    <li>Load the dynamic linker (ld-linux.so) and simulate it in order to load the libraries.  This is the most accurate
     *        since it delegates the dynamic linking to the actual dynamic linker.  It thus allows different linkers to be
     *        used.</li>
     *    <li>Use Matt Brown's work to have ROSE itself resolve the dynamic linking issues.  This approach gives us better
     *        control over the finer details such as which directories are searched, etc. since we have total control over the
     *        linker.  However, Matt's work is not complete at this time [2010-07-20].</li>
     *  </ul>
     *
     *  We use the first approach. */
    SgAsmGenericHeader* load(const char *name);

    /** Returns instruction at current IP, disassembling it if necessary, and caching it. */
    SgAsmx86Instruction *current_insn();

    /* Returns an argument of a system call */
    uint32_t arg(int idx);

    /* Causes a system call to return a particular value. */
    void sys_return(const RSIM_SEMANTIC_VTYPE<32> &value);

    /* Emulates a Linux system call from an INT 0x80 instruction. */
    void emulate_syscall();

    /* Print the name and arguments of a system call in a manner like strace using values in registers */
    void syscall_enter(const char *name, const char *fmt, ...);

    /* Print the name and arguments of a system call in a manner like strace using supplied valies */
    void syscall_enter(uint32_t *values, const char *name, const char *fmt, ...);

    /* Print the name and arguments of a system call in a manner like strace */
    void syscall_enterv(uint32_t *values, const char *name, const char *format, va_list *app);

    /* Print the return value of a system call in a manner like strace */
    void syscall_leave(const char *format, ...);

    /* Initializes an ArgInfo object to pass to syscall printing functions. */
    void syscall_arginfo(char fmt, uint32_t val, ArgInfo *info, va_list *ap);

    /* Returns the memory address in ROSE where the specified specimen address is located. */
    void *my_addr(uint32_t va, size_t size);

    /* Does the opposite, more or less, of my_addr(). Return a specimen virtual address that maps to the specified address in
     * the simulator.  There may be more than one, in which case we return the lowest. */
    uint32_t guest_va(void *addr, size_t nbytes);

    /* Reads a NUL-terminated string from specimen memory. The NUL is not included in the string.  If a limit is specified then
     * the returned string will contain at most this many characters (a value of zero implies no limit).  If the string cannot
     * be read, then "error" (if non-null) will point to a true value and the returned string will include the characters up to
     * the error. */
    std::string read_string(uint32_t va, size_t limit=0, bool *error=NULL);

    /* Reads a null-terminated vector of pointers to NUL-terminated strings from specimen memory.  If some sort of segmentation
     * fault or bus error would occur, then set *error to true and return all that we read, otherwise set it to false. */
    std::vector<std::string> read_string_vector(uint32_t va, bool *error=NULL);

    /* Simulates the generation of a signal for the specimen.  The signal is made pending (unless it's ignored) and delivered
     * synchronously. */
    void signal_generate(int signo);

    /* Deliver one (of possibly many) unmasked, pending signals. This must be called between simulated instructions. */
    void signal_deliver_any();

    /* Dispatch a signal. That is, emulate the specimen's signal handler or default action. This must be called between
     * simulated instructions. */
    void signal_deliver(int signo);

    /* Handles return from a signal handler. */
    void signal_return();

    /* Pause until a useful signal arrives. */
    void signal_pause();

    /* Helper functions for syscall 117, ipc() and related syscalls */
    void sys_semtimedop(uint32_t semid, uint32_t tsops_va, uint32_t nsops, uint32_t timeout_va);
    void sys_semget(uint32_t key, uint32_t nsems, uint32_t semflg);
    void sys_semctl(uint32_t semid, uint32_t semnum, uint32_t cmd, uint32_t semun);
    void sys_msgsnd(uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgflg);
    void sys_msgrcv(uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgtyp, uint32_t msgflg);
    void sys_msgget(uint32_t key, uint32_t msgflg);
    void sys_msgctl(uint32_t msqid, uint32_t cmd, uint32_t buf_va);
    void sys_shmdt(uint32_t shmaddr_va);
    void sys_shmget(uint32_t key, uint32_t size, uint32_t shmflg);
    void sys_shmctl(uint32_t shmid, uint32_t cmd, uint32_t buf_va);
    void sys_shmat(uint32_t shmid, uint32_t shmflg, uint32_t result_va, uint32_t ptr);

    /* Helper function for syscall 102, socketcall() and related syscalls */
    void sys_socket(int family, int type, int protocol);
    void sys_bind(int fd, uint32_t addr_va, uint32_t addrlen);
    void sys_listen(int fd, int backlog);

    /* Return number of instructions executed */
    size_t get_ninsns() const {
        return policy.get_ninsns();
    }
    
public: //FIXME
    void ctor();
    template<class guest_dirent_t> int getdents_syscall(int fd, uint32_t dirent_va, long sz);
    
    RSIM_Process *process;                      /* Process to which this thread belongs */
    RSIM_SemanticPolicy policy;
    RSIM_Semantics semantics;
    Disassembler *disassembler;                 /* Disassembler to use for obtaining instructions */


    /* Debugging, tracing, etc. */
    struct timeval last_report;                 /* Time of last progress report. */
    double report_interval;                     /* Minimum seconds between progress reports. */

    static const uint32_t SIGHANDLER_RETURN = 0xdeceaced;

    /* Stuff related to threads */
    uint32_t robust_list_head_va;               /* Address of robust futex list head. See set_robust_list() syscall */
    uint32_t set_child_tid;                     /* See set_tid_address(2) man page and clone() emulation */
    uint32_t clear_child_tid;                   /* See set_tid_address(2) man page and clone() emulation */

    /* Stuff related to signal handling. */
    sigaction_32 signal_action[_NSIG];          /* Simulated actions for signal handling; elmt N is signal N+1 */
    uint64_t signal_pending;                    /* Bit N is set if signal N+1 is pending */
    uint64_t signal_mask;                       /* Masked signals; Bit N is set if signal N+1 is masked */
    stack_32 signal_stack;                      /* Possible alternative stack to using during signal handling */
    bool signal_reprocess;                      /* Set to true if we might need to deliver signals (e.g., signal_mask changed) */

};

#endif /* ROSE_RSIM_Thread_H */
