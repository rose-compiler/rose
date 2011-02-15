#ifndef ROSE_RSIM_Thread_H
#define ROSE_RSIM_Thread_H

/** Class to represent a single simulated thread.
 *
 *  Each simulated thread runs in a single thread of the simulator. When a specimen creates a new simulated thread, ROSE creates
 *  a new real thread to simulate it; when the simulated thread exits, the real thread also exits.  Therefore, the methods
 *  defined for the RSIM_Thread class are intended to be called by only one thread at a time per RSIM_Thread object.
 *
 *  The RSIM_Thread object contains an RSIM_SemanticPolicy object which defines how instructions are executed. */

class RSIM_Thread {
public:
    /* Thrown by exit system calls. */
    struct Exit {
        explicit Exit(int status): status(status) {}
        int status;                             /* same value as returned by waitpid() */
    };

public:

    /** Constructs a new thread which belongs to the specified process. */
    RSIM_Thread(RSIM_Process *process)
        : process_TMP(process), policy(this), semantics(policy), report_interval(10.0),
          robust_list_head_va(0), set_child_tid(0), clear_child_tid(0),
          signal_pending(0), signal_mask(0), signal_reprocess(false) {
        ctor();
    }
    




    /**************************************************************************************************************************
     *                                  Methods used by system call simulation
     **************************************************************************************************************************/
public:
    /** Emulates a Linux system call from either an "INT 0x80" or "SYSENTER" instruction.  It needs no arguments since all
     *  necessary information about the system call is available on the simulated thread's stack.
     *
     *  At this time, all system calls are implemented in a single large "switch" statement within this method. The plan is to
     *  eventually replace the switch statement with a table-driven approach more suitable to being extended by the user at run
     *  time along with compile-time support for symbolic emulation. */
    void emulate_syscall();
    
    /** Print the name and arguments of a system call in a manner like strace using values in registers.
     *
     *  The @v name argument should be the name of the system call. The system call number will be automatically appended to the
     *  name.
     *
     *  The @p fmt is a format string describing the following arguments, one character per system call argument.  The following
     *  formats are allowed:
     * 
     *  <ul>
     *    <li>"-" does not print an argument value, but rather indicates that the argument value is immaterial to the call.
     *        This is generally only used when unused arguments occur between other used arguments, and not when unused
     *        arguments are the final arguments.</li>
     *    <li>"b" is used when the argument is a pointer to a character buffer. An size_t argument should follow in the
     *        varargs to indicate the total size of the buffer in bytes. In this case, print_buffer() is called to
     *        display the argument value, escaping unprintable characters, and printing an elipsis if the buffer is large.</li>
     *    <li>"d" prints the argument as a signed decimal number.</li>
     *    <li>"e" interprets the argument as an enum constant. A pointer to a Translation array should appear as the next
     *        vararg and will be used to convert the numeric argument value into a string.  If the numeric value does not appear
     *        in the Translation, then the numeric value is printed in place of a string.</li>
     *    <li>"f" interprets the argument as a set of flag bits. A pinter to a Translation arrray should appear as the next
     *        vararg and will be used to convert the numeric value into a bit-wise OR expression containing symbols from
     *        the Translation object.</li>
     *    <li>"p" interprets the argument as a void pointer and prints a hexadecimal value or the word "null".</li>
     *    <li>"P" interprets the argument as a typed pointer and prints the pointer value followed by the output from
     *        a supplied rendering function within curly braces. Two arguments should appear in the varargs list: the size of
     *        the buffer to which the pointer points, and a function that will render the content of the buffer.</li>
     *    <li>"s" interprets the argument as a pointer to a NUL-terminated ASCII string and prints the string in a C-like
     *        syntax.  If the string is long, it will be truncated and followed by an ellipsis.</li>
     *    <li>"t" interprets the argument as a time_t value and prints a human-readable date and time.</li>
     *    <li>"x" interprets the argument as an unsigned integer which is printed in hexadecimal format.</li>
     *    <li>Any other format letter will cause an immediate abort.<li>
     *  </ul>
     *
     *  This method produces no output unless system call tracing (TRACE_SYSCALL) is enabled.
     */
    void syscall_enter(const char *name, const char *fmt, ...);

    /** Print the name and arguments of a system call in a manner like strace using supplied valies.  This is identical to the
     *  other syscall_enter() method, except instead of obtaining values from the simulated thread's stack, they are supplied by
     *  the caller. */
    void syscall_enter(uint32_t *values, const char *name, const char *fmt, ...);

    /** Print the name and arguments of a system call in a manner like strace.  This is intended primarily as an internal
     *  function called by the syscall_enter() methods. */
    void syscall_enterv(uint32_t *values, const char *name, const char *format, va_list *app);

    /** Returns an argument of a system call. Arguments are numbered beginning at zero. Argument -1 is the syscall number
     * itself (which might only be valid until syscall_return() is invoked. */
    uint32_t syscall_arg(int idx);

    //@{
    /** Sets the return value for a system call.  The system call does not actually return by calling this function, it only
     *  sets the value which will eventually be returned. */
    void syscall_return(const RSIM_SEMANTIC_VTYPE<32> &value);
    void syscall_return(int value);
    //@}

    /** Print the return value of a system call in a manner like strace.  The format is the same as for the syscall_enter()
     *  methods except the first letter refers to the system call return value (the remaining letters are the arguments). The
     *  system call return value is that which was set by the syscall_return() method; the arguments are obtained via the
     *  syscall_arg() method.
     *
     *  If the first format character is "d" and the system call return value is negative and has an absolute value equal to one
     *  of the error numbers (from errno.h), then the error symbol and message are printed instead of a decimal integer.  If
     *  argument format letters are present (other than "-" placeholders), the arguments are printed on lines after the
     *  syscall_enter() line.  The most common reason for printing arguments during syscall_leave() is to show values that the
     *  operating system is returning to the user (e.g., the buffer of a read() call).
     *
     *  The system call simulation code should not output other data to the tracing file between the syscall_enter() and
     *  syscall_leave() invocations since doing so would mess up the output format.
     *
     *  This method produces no output unless system call tracing (TRACE_SYSCALL) is enabled. */
    void syscall_leave(const char *format, ...);




    /**************************************************************************************************************************
     *                                  Methods dealing with signal handling
     **************************************************************************************************************************/
public:

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


    /**************************************************************************************************************************
     *                                  Methods that help simulate system calls
     **************************************************************************************************************************/
public:

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

    int sys_clone(unsigned clone_flags, uint32_t newsp, uint32_t parent_tid_va, uint32_t child_tid_va, uint32_t pt_regs_va);

    /**************************************************************************************************************************
     *                                  Methods dealing with instruction disassembly
     **************************************************************************************************************************/
public:

    /* Return number of instructions executed */
    size_t get_ninsns() const {
        return policy.get_ninsns();
    }

    /** Returns instruction at current IP, disassembling it if necessary, and caching it.  Since the simulated memory belongs
     *  to the entire RSIM_Process, all this method does is obtain the thread's current instruction address and then has the
     *  RSIM_Process disassemble the instruction. */
    SgAsmx86Instruction *current_insn();


    /**************************************************************************************************************************
     *                                  Miscellaneous methods
     **************************************************************************************************************************/
public:

    /** Obtain a pointer to the process of which this thread is a part. The caller can expect that the owner process of a thread
     *  does not change during the lifetime of the thread. */
    RSIM_Process *get_process() const {
        assert(process_TMP!=NULL);
        return process_TMP;
    }

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



    /**************************************************************************************************************************
     *                                  Internal methods
     **************************************************************************************************************************/
protected:

    /** Constructor helper method. Called only during object construction. */
    void ctor();

    /** Initializes an ArgInfo object to pass to syscall printing functions.  This is called internally by the syscall_enter()
     *  and syscall_leave() methods. */
    void syscall_arginfo(char fmt, uint32_t val, ArgInfo *info, va_list *ap);

    /**************************************************************************************************************************
     *                                  Data members
     **************************************************************************************************************************/
protected:
    RSIM_Process *process_TMP;                      /**< Process to which this thread belongs */






    

public: //FIXME
    template<class guest_dirent_t> int getdents_syscall(int fd, uint32_t dirent_va, size_t sz);
    
    RSIM_SemanticPolicy policy;
    RSIM_Semantics semantics;


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
