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

    /** Constructs a new thread which belongs to the specified process.  RSIM_Thread objects should only be constructed by the
     *  thread that will be simulating the speciment's thread described by this object. */
    RSIM_Thread(RSIM_Process *process)
        : process(process), my_tid(-1),
          mesg_prefix(this), report_interval(10.0),
          policy(this), semantics(policy),
          robust_list_head_va(0), clear_child_tid(0) {
        real_thread = pthread_self();
        ctor();
    }



    /***************************************************************************************************************************
     *                                  Thread synchronization (simulator threads)
     ***************************************************************************************************************************/
private:
    /** Process to which this thread belongs. An RSIM_Thread object can be in an orphaned state after it exits, in which case
     *  the process will be a null pointer. */
    RSIM_Process *process;

public:
    /** Obtain a pointer to the process of which this thread is a part. The caller can expect that the owner process of a thread
     *  does not change during the lifetime of the thread, except it may become null after the thread exits. */
    RSIM_Process *get_process() const {
        assert(process!=NULL);
        return process;
    }

    /** Main loop. This loop simulates a single specimen thread and returns when the simulated thread exits. */
    void *main();

    /** Indicates whether the instruction semaphore was posted.  If an instruction needs to post the insn_semaphore, then it
     *  can do so only if insn_semaphore_posted is clear, and it must set insn_semaphore_posted.  The insn_semaphore_posted is
     *  part of RSIM_Thread because each simulator thread must have its own copy. */
    bool insn_semaphore_posted;

    /**************************************************************************************************************************
     *                                  Thread simulation (specimen threads)
     **************************************************************************************************************************/
private:
    /** The real thread that is simulating this specimen thread.  Valid until the "process" data member is null. */
    pthread_t real_thread;
    
    /** The TID of the real thread that is simulating the specimen thread described by this RSIM_Thread object.  Valid until
     * the "process" data member is null. */
    pid_t my_tid;

    /** Like a TID, but a small sequence number instead. This is more readable in error messages, and is what the id() method
     * returns. */
    static size_t next_sequence_number;
    size_t my_seq;

    /** Load the specified TLS descriptor into the GDT.  The @p idx is the index of the TLS descriptor within this thread
     * (unlike the linux kernel's set_tls_desc() whose idx is with respect to the GDT). */
    void tls_set_desc(int idx, const user_desc_32 *info);

    /** Find a free entry in this thread's TLS array and return an index into that array. This is similar to the get_free_idx()
     *  function in the Linux kernel, except this one returns an index in the thread's TLS array rather than an index in the
     *  GDT.   Returns negative on error (-ESRCH). */
    int get_free_tls() const;

    /** Global descriptor table entries which the thread overrides.   These are zero-indexed in the thread, but relative to
     * GDT_ENTRY_TLS_MIN in the process. */
    user_desc_32 tls_array[RSIM_Process::GDT_ENTRY_TLS_ENTRIES];

public:
    /** Return a thread sequence number.  Thread IDs are assigned by the Linux kernel and are akin to process IDs, and are not
     * sequential within a process. On the other hand, thread sequence numbers are small, zero-origin identifiers that we can
     * use for debug and trace messages. Sequence numbers are unique within a process. */
    int get_seq() { return my_seq; }

    /** Return the thread ID. Since each simulated thread is mapped to a unique real thread, the ID of the real thread also
     *  serves as the ID of the simulated thread.
     *
     *  Thread safety: This can be called by any thread and will always return the TID of the real thread simulating this
     *  RSIM_Thread. */
    int get_tid() { return my_tid; }

    /** Returns the POSIX thread object describing the real thread that's simulating this specimen thread. */
    pthread_t get_real_thread() const { assert(process!=NULL); return real_thread; }
    
    /** Assigns a value to one of the thread TLS array elements (part of the GDT). Returns the index number on success,
     *  negative on failure.  If info's entry_number is -1 then this method chooses an empty TLS slot and updates
     *  entry_number. */
    int set_thread_area(user_desc_32 *info, bool can_allocate);

    /** Copy the specified descriptor into a slot of the GDT. The actual slot to which we copy is stored in either the
     *  RSIM_Process or the RSIM_Thread, depending on whether idx refers to a TLS entry.  Returns the entry number on success,
     *  or negative on failure. */
    int set_gdt(const user_desc_32*);

    /** Return a pointer to an entry of the GDT.  The returned pointer might be pointing into the RSIM_Process gdt table or
     *  into the RSIM_Threads tls_array, depending on the value of @p idx. */
    user_desc_32 *gdt_entry(int idx);

    /** Traverse the robust futex list and handle futex death for each item on the list. See the Linux version of this function
     *  for details. */
    int exit_robust_list();

    /** Handle futex death.  This function is invoked for each futex on the robust list if it's owned by the calling
     * thread. See the Linux version of this function for details. */
    int handle_futex_death(uint32_t futex_va, RTS_Message*);

    /** Simulate thread exit. Return values is that which would be returned as the status for waitpid. */
    int sys_exit(const RSIM_Process::Exit &e);

    /** Performs the clear_child_tid actions. Namely, writes zero to the specified address and wakes the futex at that
     *  address.  This should be called when the child exits for any reason, or when the child calls exec. */
    void do_clear_child_tid();



    /**************************************************************************************************************************
     *                                  Debugging and tracing
     **************************************************************************************************************************/
private:
    class Prefix: public RTS_Message::Prefix {
        RSIM_Thread *thread;
    public:
        Prefix(RSIM_Thread *thread)
            : thread(thread) {}
        virtual void operator()(FILE *f) {
            fputs(thread->id().c_str(), f);
        }
    };

    Prefix mesg_prefix;
    RTS_Message *trace_mesg[TRACE_NFACILITIES];         /**< Array indexed by TraceFacility */
    struct timeval last_report;                         /**< Time of last progress report for TRACE_PROGRESS */
    double report_interval;                             /**< Minimum seconds between progress reports for TRACE_PROGRESS */

    /** Return a string identifying the thread and time called. */
    std::string id();

public:
    /** Return the object used for a debugging facility.  The return value is always non-null, although the returned message
     *  object may have a null output file if the facility is disabled.  This permits the return value to be dereferenced
     *  regardless of whether the facility is enable. For example:
     *
     *  @code
     *  tracing(TRACE_SIGNAL)->mesg("signal generated");
     *  @endcode
     *
     *  Each facility has its own message object so that multipart output can be mixed. */
    RTS_Message *tracing(TracingFacility);

    /** Print a progress report if progress reporting is enabled and enough time has elapsed since the previous report. */
    void report_progress_maybe();

    /** Prints information about stack frames.  The first line of output will include the optional title string (followed by a
     *  line feed) or the string "stack frames:".  The stack is unwound beginning with the current EIP, and using the EBP
     *  register to locate both the next EBP and the return address.  If @p bp_not_saved is true, then the return address of
     *  the inner-most function is assumed to be on the top of the stack (ss:[esp]), as is the case immediately after a CALL
     *  instruction before the called function has a chance to "PUSH EBP; MOV EBP, ESP". */
    void report_stack_frames(RTS_Message*, const std::string &title="", bool bp_not_saved=false);

    /**************************************************************************************************************************
     *                                  Callbacks
     **************************************************************************************************************************/
private:
    RSIM_Callbacks callbacks;                           /**< Callbacks per thread */

public:
    /** Obtain the set of callbacks for this object.
     *
     *  @{ */
    RSIM_Callbacks &get_callbacks() {
        return callbacks;
    }
    const RSIM_Callbacks &get_callbacks() const {
        return callbacks;
    }
    /** @} */

    /** Set all callbacks for this thread.  Note that callbacks can be added or removed individually by invoking methods on the
     *  callback object returned by get_callbacks().
     *
     *  Thread safety: This method is not thread safe. It should be called only internally by the simulator. */
    void set_callbacks(const RSIM_Callbacks &cb) {
        callbacks = cb;
    }

    /** Install a callback object.
     *
     *  This is just a convenient way of installing a callback object.  It appends it to the BEFORE slot of the appropriate
     *  queue.
     *
     *  @{ */  // ******* Similar functions in RSIM_Simulator and RSIM_Process ******
    void install_callback(RSIM_Callbacks::InsnCallback *cb) {
        callbacks.add_insn_callback(RSIM_Callbacks::BEFORE, cb);
    }
    void install_callback(RSIM_Callbacks::MemoryCallback *cb) {
        callbacks.add_memory_callback(RSIM_Callbacks::BEFORE, cb);
    }
    void install_callback(RSIM_Callbacks::SyscallCallback *cb) {
        callbacks.add_syscall_callback(RSIM_Callbacks::BEFORE, cb);
    }
    void install_callback(RSIM_Callbacks::SignalCallback *cb) {
        callbacks.add_signal_callback(RSIM_Callbacks::BEFORE, cb);
    }
    void install_callback(RSIM_Callbacks::ThreadCallback *cb) {
        callbacks.add_thread_callback(RSIM_Callbacks::BEFORE, cb);
    }
    void install_callback(RSIM_Callbacks::ProcessCallback *cb) {
        callbacks.add_process_callback(RSIM_Callbacks::BEFORE, cb);
    }
    /** @} */



    /**************************************************************************************************************************
     *                                  System call simulation
     **************************************************************************************************************************/
public:
    /** Info passed between syscall_X_enter(), syscall_X(), and syscall_X_leave() for a single system call. */
    union {
        int signo;              /* RSIM_Linux32 syscall_pause */
    } syscall_info;

    /** Emulates a Linux system call from either an "INT 0x80" or "SYSENTER" instruction.  It needs no arguments since all
     *  necessary information about the system call is available on the simulated thread's stack.
     *
     *  At this time, all system calls are implemented in a single large "switch" statement within this method. The plan is to
     *  eventually replace the switch statement with a table-driven approach more suitable to being extended by the user at run
     *  time along with compile-time support for symbolic emulation. */
    void emulate_syscall();
    
    /** Print the name and arguments of a system call in a manner like strace using values in registers.
     *
     *  The @p name argument should be the name of the system call. The system call number will be automatically appended to the
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
     *    <li>"D" is reserved for use by syscall_leave().</li>
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

    /** Print the name and arguments of a system call in a manner like strace using supplied values.  This is identical to the
     *  other syscall_enter() method, except instead of obtaining values from the simulated thread's stack, they are supplied by
     *  the caller. */
    void syscall_enter(uint32_t *values, const char *name, const char *fmt, ...);

    /** Print the name and arguments of a system call in a manner like strace.  This is intended primarily as an internal
     *  function called by the syscall_enter() methods. */
    void syscall_enterv(uint32_t *values, const char *name, const char *format, va_list *app);

    /** Returns an argument of a system call. Arguments are numbered beginning at zero. Argument -1 is the syscall number
     * itself (which might only be valid until syscall_return() is invoked. */
    uint32_t syscall_arg(int idx);

    /** Sets the return value for a system call.  The system call does not actually return by calling this function, it only
     *  sets the value which will eventually be returned.
     *
     *  @{ */
    void syscall_return(const RSIM_SEMANTIC_VTYPE<32> &value);
    void syscall_return(int value);
    /** @} */

    /** Print the return value of a system call in a manner like strace.  The format is the same as for the syscall_enter()
     *  methods except the first letter refers to the system call return value (the remaining letters are the arguments). The
     *  system call return value is that which was set by the syscall_return() method; the arguments are obtained via the
     *  syscall_arg() method.
     *
     *  If the first format character is "d" or "D" and the system call return value is negative and has an absolute value
     *  equal to one of the error numbers (from errno.h), then the error symbol and message are printed instead of a decimal
     *  integer.  If the first character is "D" then the second character serves as the format of the return value when an
     *  error number is not returned, and following formats are for the arguments. If argument format letters are present
     *  (other than "-" placeholders), the arguments are printed on lines after the syscall_enter() line.  The most common
     *  reason for printing arguments during syscall_leave() is to show values that the operating system is returning to the
     *  user (e.g., the buffer of a read() call).
     *
     *  The system call simulation code should not output other data to the tracing file between the syscall_enter() and
     *  syscall_leave() invocations since doing so would mess up the output format.
     *
     *  This method produces no output unless system call tracing (TRACE_SYSCALL) is enabled. */
    void syscall_leave(const char *format, ...);

    /** Print the return value of a system call in a manner like strace but using supplied values. This is identical to the
     * other syscall_leave() method, except instead of obtaining values from the simulated thread's stack, they are supplied by
     * the caller. */
    void syscall_leave(uint32_t *values, const char *format, ...);

    /** Print the return value of a system call. This is intended primarily as an internal function called by the various
     * syscall_leave() methods. */
    void syscall_leavev(uint32_t *values, const char *name, va_list *app);



    /**************************************************************************************************************************
     *                                  Signal handling
     **************************************************************************************************************************/
private:
    RSIM_SignalHandling sighand;
    static const uint32_t SIGHANDLER_RETURN = 0xdeceaced;               /* arbitrary magic number */
    static const uint32_t SIGHANDLER_RT_RETURN = 0xdecea5ed;            /* arbitrary magic number */

public:
    /** Removes a non-masked signal from the thread's signal queue. If the thread's queue is empty then the process' queue is
     *  considered. Returns a signal number if one is removed, zero if no signal is available, negative on error.  If a
     *  positive signal number is returned then the supplied siginfo_32 struct is filled in with information about that
     *  signal. */
    int signal_dequeue(RSIM_SignalHandling::siginfo_32 *info/*out*/);

    /** Cause a signal to be delivered. The signal is not removed from the pending set or signal queue, nor do we check whether
     *  the signal is masked.  Returns zero on success, negative errno on failure.  However, if the signal is a terminating
     *  signal whose default action is performed, this method will throw an Exit, which will cause all simulated threads to
     *  shut down and the simulator returns to user control. */
    int signal_deliver(const RSIM_SignalHandling::siginfo_32&);

    /** Handles return from a signal handler. Returns zero on success, negative errno on failure. The only failure that is
     *  detected at this time is -EFAULT when reading the signal handler stack frame, in which case a message is printed to
     *  TRACE_SIGNAL and no registers or memory are modified.
     *
     *  @{ */
    int sys_sigreturn();
    int sys_rt_sigreturn();
    /** @} */

    /** Accepts a signal from the process manager for later delivery.  This function is called by RSIM_Process::sys_kill() to
     *  decide to which thread a signal should be delivered.  If the thread can accept the specified signal, then it does so,
     *  adding the signal to its queue.  A signal can be accepted by this thread if the signal is not blocked.
     *
     *  Returns zero if the signal was accepted; negative if the signal was not accepted. */
    int signal_accept(const RSIM_SignalHandling::siginfo_32&);

    /** Returns, through an argument, the set of signals that are pending.  Returns zero on success, negative errno on
     * failure. */
    int sys_sigpending(RSIM_SignalHandling::sigset_32 *result);

    /** Sends a signal to a thread or process.  If @p tid is negative, then the signal is send to the specified process, which
     *  then delivers it to one of its threads.  Otherwise the signal is sent to the specified thread of the specified
     *  process.
     *
     *  Signals that are destined for the calling thread are simply placed on the calling thread's queue and will be handled at
     *  the next opportunity.  Signals destined for another thread of the calling process are placed on that thread's queue and
     *  that thread is sent the RSIM_SignalHandling::SIT_WAKEUP signal to any blocking system call in that thread
     *  returns. Signals destined for an entire process are handled by RSIM_Process::sys_kill().
     *
     *  Returns non-negative on success; negative error number on failure.
     *
     *  Thread safety: This function can be called by multiple threads concurrently if they all use different RSIM_Thread
     *  objects. This function is not async-signal safe. */
    int sys_tgkill(pid_t pid, pid_t tid, int signo);

    /** Sends a signal to an entire process.  This is really just a convenience function for RSIM_Process::sys_kill(). It
     *  returns non-negative on success; negative error number on failure.
     *
     *  Thread safety: This function is thread safe. */
    int sys_kill(pid_t pid, const RSIM_SignalHandling::siginfo_32&);

    /** Block until a signal arrives.  Temporarily replaces the signal mask with the specified mask, then blocks until the
     *  delivery of a signal whose action is to invoke a signal handler or terminate the specimen. On success, returns the
     *  number of the signal that caused this function to return; returns negative on failure. (Note that the return value is
     *  not the same as the real sigsuspend.)
     *
     *  FIXME: This doesn't have quite the same semantics as the real sigsuspend(): the signal handler is invoked after this
     *  method returns and after the original signal mask has been restored. [RPM 2011-03-08]
     *
     *  Thread safety: This method is thread safe. */
    int sys_sigsuspend(const RSIM_SignalHandling::sigset_32 *mask);

    /** Get and/or set the signal mask for a thread. Returns negative errno on failure.
     *
     *  Thread safety: This method is thread safe. */
    int sys_sigprocmask(int how, const RSIM_SignalHandling::sigset_32 *in, RSIM_SignalHandling::sigset_32 *out);

    /** Defines a new alternate signal stack and/or retrieve the state of an existing alternate signal stack.  This method is
     *  similar in behavior to the real sigaltstack() function, except the return value is zero on success or a negative error
     *  number on failure.  Each thread has its own alternate signal stack property.
     *
     *  It is not permissible to change the signal stack while it's being used. The @p sp argument is used only for checking
     *  whether the stack is currently in use.
     *
     *  Thread safety: This method is thread safe. */
    int sys_sigaltstack(const stack_32 *in, stack_32 *out);
    

    /**************************************************************************************************************************
     *                                  Futex interface
     **************************************************************************************************************************/
public:
    
    /** Wait for a futex.  Verifies that the aligned memory at @p va contains the value @p oldval and sleeps, waiting
     * sys_futex_wake on this address.  Returns zero on success; negative error number on failure.  See manpage futex(2) for
     * details about the return value, although the return values here are negative error numbers rather than -1 with errno
     * set.
     *
     * The @p bitset value is a bit vector added to the wait queue.  When waking threads that are blocked, we wake only those
     * threads where the intersection of the wait and wake bitsets is non-empty.
     *
     * Thread safety:  This method is thread safe. */
    int futex_wait(rose_addr_t va, uint32_t oldval, uint32_t bitset=0xffffffff);

    /** Wakes blocked processes.  Wakes at most @p nprocs processes waiting for the specified address.  Returns the number of
     * processes woken up; negative error number on failure.
     *
     * The @p bitset value is a bit vector used when waking blocked threads.  Only threads where the intersection of the wait
     * bitset with this wake bitset is non-empty are awoken.
     *
     * Thread safety:  This method is thread safe. */
    int futex_wake(rose_addr_t va, int nprocs, uint32_t bitset=0xffffffff);
    

    /**************************************************************************************************************************
     *                                  Instruction disassembly
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

    /** Obtain current register values. */
    pt_regs_32 get_regs() const;

    /** Initialize registers */
    void init_regs(const pt_regs_32 &regs);


    /**************************************************************************************************************************
     *                                  Internal methods
     **************************************************************************************************************************/
protected:

    /** Constructor helper method. Called only during object construction. */
    void ctor();

    /** Initializes an ArgInfo object to pass to syscall printing functions.  This is called internally by the syscall_enter()
     *  and syscall_leave() methods. */
    void syscall_arginfo(char fmt, uint32_t val, ArgInfo *info, va_list *ap);

public:
    void post_fork();           /**< Kludge for now. */

    /**************************************************************************************************************************
     *                                  Data members
     **************************************************************************************************************************/

public: //FIXME
    template<class guest_dirent_t> int getdents_syscall(int fd, uint32_t dirent_va, size_t sz);
    
    RSIM_SemanticPolicy policy;
    RSIM_Semantics semantics;


    /* Stuff related to threads */
    uint32_t robust_list_head_va;               /* Address of robust futex list head. See set_robust_list() syscall */
    uint32_t clear_child_tid;                   /* See set_tid_address(2) man page and clone() emulation */


};

#endif /* ROSE_RSIM_Thread_H */
