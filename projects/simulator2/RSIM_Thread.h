#ifndef ROSE_RSIM_Thread_H
#define ROSE_RSIM_Thread_H

#include "RSIM_Semantics.h"
#include "RSIM_Process.h"

/** Class to represent a single simulated thread.
 *
 *  Each simulated thread runs in a single thread of the simulator. When a specimen creates a new simulated thread, ROSE creates
 *  a new real thread to simulate it; when the simulated thread exits, the real thread also exits.  Therefore, the methods
 *  defined for the RSIM_Thread class are intended to be called by only one thread at a time per RSIM_Thread object. */
class RSIM_Thread {
public:
    enum RunState { INITIALIZING, RUNNING, TERMINATED };

private:
    SAWYER_THREAD_TRAITS::RecursiveMutex mutex_;        // for data accessible from other threads
    SAWYER_THREAD_TRAITS::ConditionVariable runStateChanged_;
    RunState runState_;
    RSIM_Semantics::DispatcherPtr dispatcher_;
    
public:

    /** Constructs a new thread which belongs to the specified process.  RSIM_Thread objects should only be constructed by the
     *  thread that will be simulating the speciment's thread described by this object. */
    RSIM_Thread(RSIM_Process *process, boost::thread &hostThread)
        : runState_(INITIALIZING), process(process),
#if __cplusplus < 201103L
          real_thread(boost::detail::thread_move_t<boost::thread>(hostThread)),
#else
          real_thread(std::move(hostThread)),
#endif
          my_tid(-1),
          report_interval(10.0), do_coredump(true), show_exceptions(true),
          robustListHeadVa_(0), clearChildTidVa_(0) {
        memset(trace_mesg, 0, sizeof trace_mesg);
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

    /** Cause a thread to start running.  This must be called after a thread is created in order for it to start doing
     * anything.
     *
     * Thread safety: This can be called from any thread. */
    void start();

    /** Wait for thread to enter specific state. */
    void waitForState(RunState);

    /** Set thread to a specific state and notify waiters. */
    void setState(RunState);

    /** Main loop. This loop simulates a single specimen thread and returns when the simulated thread exits. */
    void *main();

    /** Indicates whether the instruction semaphore was posted.  If an instruction needs to post the insn_semaphore, then it
     *  can do so only if insn_semaphore_posted is clear, and it must set insn_semaphore_posted.  The insn_semaphore_posted is
     *  part of RSIM_Thread because each simulator thread must have its own copy. */
    bool insn_semaphore_posted;

    /** Post the instruction semaphore if not already posted. */
    void post_insn_semaphore();

    /**************************************************************************************************************************
     *                                  Thread simulation (specimen threads)
     **************************************************************************************************************************/
private:
    /** The real thread that is simulating this specimen thread.  Valid until the "process" data member is null. */
    boost::thread real_thread;
    
    /** The TID of the real thread that is simulating the specimen thread described by this RSIM_Thread object.  Valid until
     * the "process" data member is null.   This value is updated for the main thread of a child process after a fork (see
     * RSIM_Process::atfork_child(). */
    pid_t my_tid;

    /** Like a TID, but a small sequence number instead. This is more readable in error messages, and is what the id() method
     * returns. */
    static size_t next_sequence_number;
    size_t my_seq;

    /** Load the specified TLS descriptor into the GDT.  The @p idx is the index of the TLS descriptor within this thread
     * (unlike the linux kernel's set_tls_desc() whose idx is with respect to the GDT). */
    void tls_set_desc(int idx, const SegmentDescriptor &info);

    /** Find a free entry in this thread's TLS array and return an index into that array. This is similar to the get_free_idx()
     *  function in the Linux kernel, except this one returns an index in the thread's TLS array rather than an index in the
     *  GDT.   Returns negative on error (-ESRCH). */
    int get_free_tls() const;

    /** Global descriptor table entries which the thread overrides.   These are zero-indexed in the thread, but relative to
     * GDT_ENTRY_TLS_MIN in the process. */
    SegmentDescriptor tls_array[RSIM_Process::GDT_ENTRY_TLS_ENTRIES];

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

    /** Initialize the thread ID.  The specimen thread ID is the same as the real simulator thread that "owns" the specimen
     *  thread.  The thread ID should not be changed except by parts of the RSIM_Process that understand how threads are looked
     *  up, since the process typically uses an std::map indexed by thread ID.
     *
     *  Thread safety: Not thread safe, but this is normally called by initialization functions before the thread is known to
     *  other parts of the simulator. */
    void set_tid();

    /** Returns the thread object describing the real thread that's simulating this specimen thread. */
    boost::thread& get_real_thread() { assert(process!=NULL); return real_thread; }
    
    /** Assigns a value to one of the thread TLS array elements (part of the GDT). Returns the index number on success,
     *  negative on failure.  If info's entry_number is -1 then this method chooses an empty TLS slot and updates
     *  entry_number. */
    int set_thread_area(SegmentDescriptor &info, bool can_allocate);

    /** Copy the specified descriptor into a slot of the GDT. The actual slot to which we copy is stored in either the
     *  RSIM_Process or the RSIM_Thread, depending on whether idx refers to a TLS entry.  Returns the entry number on success,
     *  or negative on failure. */
    int set_gdt(const SegmentDescriptor&);

    /** Return a reference to an entry of the GDT.  The returned reference might be pointing into the RSIM_Process gdt table or
     *  into the RSIM_Threads tls_array, depending on the value of @p idx. */
    SegmentDescriptor& gdt_entry(int idx);

    /** Obtain current register values. */
    PtRegs get_regs() const;

    /** Initialize registers */
    void init_regs(const PtRegs &regs);

    /** Traverse the robust futex list and handle futex death for each item on the list. See the Linux version of this function
     *  for details. */
    int exit_robust_list();

    /** Handle futex death.  This function is invoked for each futex on the robust list if it's owned by the calling
     * thread. See the Linux version of this function for details. */
    int handle_futex_death(uint32_t futex_va, Sawyer::Message::Stream&);

    /** Simulate thread exit. Return values is that which would be returned as the status for waitpid. */
    int sys_exit(const RSIM_Process::Exit &e);

    /** Performs the clear_child_tid actions. Namely, writes zero to the specified address and wakes the futex at that
     *  address.  This should be called when the child exits for any reason, or when the child calls exec. */
    void do_clear_child_tid();



    /**************************************************************************************************************************
     *                                  Debugging and tracing
     **************************************************************************************************************************/
private:
    Sawyer::Message::Stream *trace_mesg[TRACE_NFACILITIES];
    struct timeval last_report;                         /**< Time of last progress report for TRACE_PROGRESS */
    double report_interval;                             /**< Minimum seconds between progress reports for TRACE_PROGRESS */
    bool do_coredump;                                   /**< Simulatate a core dump (when true), or throw an exception. */
    bool show_exceptions;                               /**< Print an error message about exceptions in RSIM_Thread::main()? */

    /** Return a string identifying the thread and time called. */
    std::string id();

public:
    /** Return the object used for a debugging facility.  The return value is always non-null, although the returned message
     *  object may be disabled. For example:
     *
     *  @code
     *  tracing(TRACE_SIGNAL) <<"signal generated\n";
     *  @endcode
     *
     *  Each facility has its own message object so that multipart output can be mixed. */
    Sawyer::Message::Stream& tracing(TracingFacility);

    /** Print a progress report if progress reporting is enabled and enough time has elapsed since the previous report. */
    void report_progress_maybe();

    /** Prints information about stack frames.  The first line of output will include the optional title string (followed by a
     *  line feed) or the string "stack frames:".  The stack is unwound beginning with the current EIP, and using the EBP
     *  register to locate both the next EBP and the return address.  If @p bp_not_saved is true, then the return address of
     *  the inner-most function is assumed to be on the top of the stack (ss:[esp]), as is the case immediately after a CALL
     *  instruction before the called function has a chance to "PUSH EBP; MOV EBP, ESP". */
    void report_stack_frames(Sawyer::Message::Stream&, const std::string &title="", bool bp_not_saved=false);

    /** Create, open, or reassign all tracing facilities.  This method is called when a thread is constructed or when a thread
     *  is reassigned to a new process after a fork.  For each tracing facility, a message stream is created (if necessary) to
     *  point to either the specified file or no file, depending on whether the corresponding trace flag is set for the
     *  containing process.  If an message stream already exists, then its output is directed to the specified C++ stream. */
    void reopen_trace_facilities();

    /** The do_core_dump property determines whether a failing thread should simulate a specimen core dump or throw an
     *  exception. When the property is false, an exception is thrown.  @{ */
    bool get_do_coredump() const { return do_coredump; }
    void set_do_coredump(bool b=true) { do_coredump = b; }
    void clear_do_coredump() { set_do_coredump(false); }
    /** @} */

    /** The show_exceptions property determines whether exceptions caught by RSIM_Thread::main() are displayed to the
     *  TRACE_MISC output stream. The default is true.
     * @{ */
    bool get_show_exceptions() const { return show_exceptions; }
    void set_show_exceptions(bool b=true) { show_exceptions = b; }
    void clear_show_exceptions() { set_show_exceptions(false); }
    /** @} */

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
     *  This is just a convenient way of installing a callback object.  It appends it to the BEFORE slot (by default) of the
     *  appropriate queue.
     *
     *  @{ */  // ******* Similar functions in RSIM_Simulator and RSIM_Process ******
    void install_callback(RSIM_Callbacks::InsnCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.add_insn_callback(when, cb);
    }
    void install_callback(RSIM_Callbacks::MemoryCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.add_memory_callback(when, cb);
    }
    void install_callback(RSIM_Callbacks::SyscallCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.add_syscall_callback(when, cb);
    }
    void install_callback(RSIM_Callbacks::SignalCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.add_signal_callback(when, cb);
    }
    void install_callback(RSIM_Callbacks::ThreadCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.add_thread_callback(when, cb);
    }
    void install_callback(RSIM_Callbacks::ProcessCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.add_process_callback(when, cb);
    }
    /** @} */

    /** Remove a callback object.
     *
     *  This is just a convenient way of removing callback objects.  It removes up to one instance of the callback from the
     *  thread.  The comparison to find a callback object is by callback address.
     *
     * @{ */
    void remove_callback(RSIM_Callbacks::InsnCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.remove_insn_callback(when, cb);
    }
    void remove_callback(RSIM_Callbacks::MemoryCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.remove_memory_callback(when, cb);
    }
    void remove_callback(RSIM_Callbacks::SyscallCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.remove_syscall_callback(when, cb);
    }
    void remove_callback(RSIM_Callbacks::SignalCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.remove_signal_callback(when, cb);
    }
    void remove_callback(RSIM_Callbacks::ThreadCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.remove_thread_callback(when, cb);
    }
    void remove_callback(RSIM_Callbacks::ProcessCallback *cb, RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE) {
        callbacks.remove_process_callback(when, cb);
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

    /** Return print helper for printing syscall arguments.
     *
     * @{ */
    Printer print(Sawyer::Message::Stream&, const std::string &atEnd);
    Printer print(Sawyer::Message::Stream&, const uint32_t *args, const std::string &atEnd);
    Printer print(Sawyer::Message::Stream&, const uint64_t *args, const std::string &atEnd);
    Printer print(TracingFacility, const std::string &atEnd);
    /** @} */

    
    /** Print the name and arguments of a system call in a manner like strace using values in registers.
     *
     *  The @p name argument should be the name of the system call. The system call number will be automatically appended to
     *  the name.  If an array of values is provided then they're used instead of querying from the thread state.
     *
     *  Returns a syscall printer for printing syscall arguments of various formats.
     *
     *  This method produces no output unless system call tracing (TRACE_SYSCALL) is enabled.
     *
     * @{ */
    Printer syscall_enter(const std::string &name);
    Printer syscall_enter(const uint32_t*, const std::string& name);
    Printer syscall_enter(const uint64_t*, const std::string& name);
    /** @} */

    /** Returns an argument of a system call. Arguments are numbered beginning at zero. Argument -1 is the syscall number
     * itself (which might only be valid until syscall_return() is invoked. */
    uint64_t syscall_arg(int idx);

    /** Sets the return value for a system call.  The system call does not actually return by calling this function, it only
     *  sets the value which will eventually be returned.
     *
     *  @{ */
    void syscall_return(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &value);
    void syscall_return(uint64_t value);
    /** @} */

    /** Print the return value of a system call in a manner like strace.
     *
     *  Returns a Printer object for printing the return value plus modified arguments.
     *
     * @{ */
    Printer syscall_leave();
    Printer syscall_leave(const uint32_t *args);
    Printer syscall_leave(const uint64_t *args);
    /** @} */


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
     *  positive signal number is returned then the supplied @p info struct is filled in with information about that
     *  signal. */
    int signal_dequeue(RSIM_SignalHandling::SigInfo *info/*out*/);

    /** Cause a signal to be delivered. The signal is not removed from the pending set or signal queue, nor do we check whether
     *  the signal is masked.  Returns zero on success, negative errno on failure.  However, if the signal is a terminating
     *  signal whose default action is performed, this method will throw an Exit, which will cause all simulated threads to
     *  shut down and the simulator returns to user control. */
    int signal_deliver(const RSIM_SignalHandling::SigInfo&);

    /** Clears all pending signals.
     *
     *  Thread safety:  This method is thread safe. */
    void signal_clear_pending();

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
    int signal_accept(const RSIM_SignalHandling::SigInfo&);

    /** Returns, through an argument, the set of signals that are pending.  Returns zero on success, negative errno on
     * failure. */
    int sys_sigpending(RSIM_SignalHandling::SigSet *result);

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
    int sys_kill(pid_t pid, const RSIM_SignalHandling::SigInfo&);

    /** Block until a signal arrives.  Temporarily replaces the signal mask with the specified mask, then blocks until the
     *  delivery of a signal whose action is to invoke a signal handler or terminate the specimen. On success, returns the
     *  number of the signal that caused this function to return; returns negative on failure. (Note that the return value is
     *  not the same as the real sigsuspend.)
     *
     *  FIXME: This doesn't have quite the same semantics as the real sigsuspend(): the signal handler is invoked after this
     *  method returns and after the original signal mask has been restored. [RPM 2011-03-08]
     *
     *  Thread safety: This method is thread safe. */
    int sys_sigsuspend(const RSIM_SignalHandling::SigSet *mask);

    /** Get and/or set the signal mask for a thread. Returns negative errno on failure.
     *
     *  Thread safety: This method is thread safe. */
    int sys_sigprocmask(int how, const RSIM_SignalHandling::SigSet *in, RSIM_SignalHandling::SigSet *out);

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
     *                                  Process forking
     **************************************************************************************************************************/
public:

    /** Fork pre/post processing.
     *
     *  Forking a multi-threaded process is fraught with danger.  The entire address space of the parent process is replicated
     *  in the child, including the states of mutexes, condition variables, and other pthreads objects, but only one thread
     *  runs in the child--the other threads appear to have simply died.  According to pthread_atfork(3) for Linux 2.6.32, "The
     *  mutexes are not usable after the fork and must be initialized with pthread_mutex_init in the child process.  This is a
     *  limitation of the current implementation and might or might not be present in future versions."
     *
     *  Since the simulator uses a variety of thread synchronization objects, we must take special care when a multi-threaded
     *  specimen (and thus multi-threaded simulator) is forked.  All forks within the simulator should be surrounded with calls
     *  to atfork_prepare(), atfork_parent(), and atfork_child(). (At the time of this writing [2012-01-20] the only fork()
     *  call is for the emulated fork system call in RSIM_Linux32.C).
     *
     *  Only one thread (per process) can be calling fork.  We enforce that through the simulator's global IPC semaphore, the
     *  same one that ensures that instruction emulation is atomic.  The semaphore is obtained in the atfork_prepare() call and
     *  released afterward by atfork_parent().  The child need not protect any data structures immediately after the fork since
     *  it will not share RSIM data structures with the parent, and will be running with only one thread.
     *
     * @{ */
    void atfork_prepare();
    void atfork_parent();
    void atfork_child();
    /** @} */


    /**************************************************************************************************************************
     *                                  Futex interface
     **************************************************************************************************************************/
public:
    
    /** Wait for a futex.  Verifies that the aligned memory at @p va contains the value @p oldval and sleeps, waiting
     *  sys_futex_wake on this address.  Returns zero on success; negative error number on failure.  See manpage futex(2) for
     *  details about the return value, although the return values here are negative error numbers rather than -1 with errno
     *  set.  May return -EINTR if interrupted by a signal, in which case internal semaphores and the futex table are restored
     *  to their original states.
     *
     *  The @p bitset value is a bit vector added to the wait queue.  When waking threads that are blocked, we wake only those
     *  threads where the intersection of the wait and wake bitsets is non-empty.
     *
     *  Thread safety:  This method is thread safe. */
    int futex_wait(rose_addr_t va, uint32_t oldval, uint32_t bitset=0xffffffff);

    /** Wakes blocked processes.  Wakes at most @p nprocs processes waiting for the specified address.  Returns the number of
     *  processes woken up; negative error number on failure.  When returning error, no processes were woken up.
     *
     *  The @p bitset value is a bit vector used when waking blocked threads.  Only threads where the intersection of the wait
     *  bitset with this wake bitset is non-empty are awoken.
     *
     *  Thread safety:  This method is thread safe. */
    int futex_wake(rose_addr_t va, int nprocs, uint32_t bitset=0xffffffff);

    /** Property: head of robust list for futexes.
     *
     * @{ */
    rose_addr_t robustListHeadVa() const { return robustListHeadVa_; }
    void robustListHeadVa(rose_addr_t va) { robustListHeadVa_ = va; }
    /** @} */

    /** Property: address where TID is cleared on exit.
     *
     *  See set_tid_address(2) man page for details.
     *
     * @{ */
    rose_addr_t clearChildTidVa() const { return clearChildTidVa_; }
    void clearChildTidVa(rose_addr_t va) { clearChildTidVa_ = va; }
    /** @} */

private:
    /** Obtain a key for a futex.  Futex queues are based on real addresses, not process virtual addresses.  In other words,
     *  the same futex can have two different addresses in two different processes.  In fact, it could even have two or more
     *  addresses in a single processes.  We can't figure out real addresses, so we map the specimen's futex address into the
     *  corresponding simulator address.  Then, in order to handle the case where the simulator's address is in shared memory,
     *  we look up the address in /proc/self/maps, and if we find it belongs to a file, we generate a hash based on the file's
     *  device and inode numbers and the offset of the futex within the file.
     *
     *  Upon successful return, val_ptr will point to the address in simulator memory where the futex value is stored.  This
     *  means that the specimen futex is required to occupy a single region of memory--it cannot span two different mapped
     *  regions.
     *
     *  Returns zero on failure, non-zero on success.
     *
     *  Thread safety:  This method is not thread safe. We are assuming that the calling function has already surrounded this
     *  call with a mutex that protects this function from being entered concurrently by any other thread of the calling
     *  process. */
    rose_addr_t futex_key(rose_addr_t va, uint32_t **val_ptr);
    

    /**************************************************************************************************************************
     *                                  Instruction disassembly
     **************************************************************************************************************************/
public:

    /* Return number of instructions executed */
    size_t get_ninsns() const {
        return operators()->nInsns();
    }

    /** Returns instruction at current IP, disassembling it if necessary, and caching it.  Since the simulated memory belongs
     *  to the entire RSIM_Process, all this method does is obtain the thread's current instruction address and then has the
     *  RSIM_Process disassemble the instruction. */
    SgAsmInstruction *current_insn();


    /***************************************************************************************************************************
     *                                  Instruction processing
     ***************************************************************************************************************************/
public:
    RSIM_Semantics::DispatcherPtr dispatcher() const {
        return dispatcher_;
    }

    RSIM_Semantics::RiscOperatorsPtr operators() const;

    Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr pop();
    
    /**************************************************************************************************************************
     *                                  Dynamic Linking
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


    /**************************************************************************************************************************
     *                                  Internal methods
     **************************************************************************************************************************/
protected:

    /** Constructor helper method. Called only during object construction. */
    void ctor();

    /**************************************************************************************************************************
     *                                  Data members
     **************************************************************************************************************************/

private:
    /* Stuff related to threads */
    rose_addr_t robustListHeadVa_;                      /* Address of robust futex list head. See set_robust_list() syscall */
    rose_addr_t clearChildTidVa_;                       /* See set_tid_address(2) man page and clone() emulation */


};

#endif /* ROSE_RSIM_Thread_H */
