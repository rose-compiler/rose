#ifndef ROSE_RSIM_SignalHandling_H
#define ROSE_RSIM_SignalHandling_H

/** Provides the functionality needed to simulate the handling of signals in a potentially multi-threaded specimen. RSIM
 *  supports the following four scenarios:
 *
 *  <ul>
 *    <li>Sending a signal from a simulated thread to any thread within the same thread group (process).  This is accomplished
 *        by simply adding the signal to the target thread's queue of pending signals and then sending a SIG_WAKEUP signal to
 *        the target thread (if it's different than the sending thread) to cause it to resume from any potential blocking system
 *        call.  The RSIM_Thread::sys_tgkill() method can be used for this.</li>
 *    <li>Sending a signal from a simulated thread to the simulated process-as-a-whole.  The simulator chooses a thread to
 *        serve as the target based on the set of signal masks across all threads of the process. The signal is added to the
 *        target threads signal queue and a SIG_WAKEUP is sent to the target thread (if different than the sending thread) to
 *        cause it to resume from any potential blocking system call.  The RSIM_Thread::sys_kill() can be used for this, as can
 *        RSIM_Thread::sys_tgkill() with a negative thread ID.</li>
 *    <li>Sending a signal from a simulated thread to a simulated or non-simulated process-as-a-whole, or to a non-simulated
 *        thread. This is done simply by calling kill() or tgkill() depending on the situation.</li>
 *    <li>Reception of a signal by the simulated process-as-a-whole, having been sent from some other simulated or
 *        non-simulated process. In this case, one of the threads' signal handlers is invoked, asynchronouwly adding the signal
 *        to the process-wide signal queue.  The next thread to reach the next iteration through its RSIM_Thread::main() loop
 *        will behave as if it were sending signals to its process as described above, removing them from the process-wide
 *        signal queue.  This two-step algorithm allows the asynchronous component to be relatively simple, and the more
 *        complicated work happening synchronously when thread synchronization primitives are in a known state.</li>
 *  </ul>
 *
 *  In particular, a signal cannot be directed to a specific specimen thread from outside its thread group. At best,
 *  interprocess signals can only be directed to simulated processes-as-a-whole (or unsimulated threads or processes).  In
 *  practice this isn't much of a restriction since inter-process, thread-to-thread communication is non-portable.
 *
 *  @section AsyncSigRcpt Asynchronous Signal Reception
 *
 *  Signals arriving from other processes are received via an asynchronous signal handler in RSIM. For each signal that is
 *  typically used for interprocess communication (i.e., generally not program error signals like SIGFPE, SIGILL, SIGSEGV,
 *  SIGBUS, and SIGABRT and not real-time signals), a signal handler is registered by RSIM_Simulator::activate().  This handler
 *  runs with all signals masked and adds the received signal to the tail of a statically allocated, circular, process-wide
 *  queue of pending signals.  Signals that arrive when the queue is full are dropped.  No attempt is made to choose a thread to
 *  handle the signal, or to decide whether the current disposition is to ignore the signal.
 *
 *  @section SyncSigRcpt Synchronous Signal Reception
 *
 *  At every iteration of the big simulation loop in RSIM_Thread::main(), every thread checks whether the process-wide queue
 *  contains signals. If so, the thread acts as if it were sending those signals to its process, removing them from the head of
 *  the queue in the order they arrived.  This operation requires a process-wide exclusive lock.
 *
 *  @section SigDeliv Signal Delivery and Return
 *
 *  "Signal Delivery" is the act of causing a simulated thread to handle a signal, either by doing its default action for that
 *  signal or by executing its user-defined signal handler.  At most one signal is handled per iteration of the
 *  RSIM_Thread::main() loop.  Real-time signals are processed in the order they arrived and ahead of classic signals. Classic
 *  signals are processed by order of signal number using a "pending" bit vector.  If two occurrences of a classic signal
 *  arrive in quick succession, the second occurrence might be dropped--this is the correct behavior for classic signals.
 *  Signals that are masked are not immediately delivered, but they do not block the delivery of other signals.
 *
 *  When a signal is available for delivery then it is removed from the queue (or pending bit vector) by
 *  RSIM_Signal::dequeue(), and processed by RSIM_Thread::signal_deliver().  The default action is handled internally by RSIM;
 *  user-defined signal handlers are invoked by optionally switching to the signal alternate stack, saving registers and the
 *  current signal mask on the stack, pushing the signal number onto the stack, pushing a special return value
 *  (SIGHANDLER_RETURN), and branching to the user-defined signal handler entry address.
 *
 *  When a user-defined signal handler returns, it will return the the special SIGHANDLER_RETURN address. RSIM notices this
 *  address and emulates system call 119, sigreturn, restoring the old signal mask and registers and resuming execution.
 *
 *  @section SpecialSigs Special Signals
 *
 *  When a signal is sent by a sending thread to a (different) target thread, the target thread must be awoken from any blocked
 *  system calls it might be executing on behalf of the specimen.  RSIM accomplishes this by sending SIG_WAKEUP, defined as an
 *  arbitrary signal (currently one of the real-time signals).  This will cause the target thread's system call to fail with
 *  EINTR and its simulated signal handler to run. RSIM is currently not capable of restarting simulated system calls.  The
 *  SIG_WAKEUP is not blocked by any thread (except possibly when another signal handler is running), and is always sent via
 *  pthread_signal().
 *
 *  The Linux Native POSIX Thread Library (NPTL) uses two real-time signals (_SIGRTMIN+0 and _SIGRTMIN+1) internally. RSIM
 *  therefore must never install signal handlers for these signals, and thus cannot detect when these signals are sent to the
 *  specimen from another process.
 *
 *  The internal use of these symbols does not preclude their use by the specimen as long as the specimen does not expect to
 *  receive these signals from another process. */
class RSIM_SignalHandling {
public:
    RSIM_SignalHandling()
        : mask(0), queue_head(0), queue_tail(0), pending(0), reprocess(0) {
        RTS_mutex_init(&mutex, RTS_LAYER_RSIM_SIGNALHANDLING_OBJ, NULL);
        memset(queue, 0, sizeof queue);
        memset(&stack, 0, sizeof stack);
        memset(&pending_info, 0, sizeof pending_info);
        stack.ss_sp = 0;
        stack.ss_size = 0;
        stack.ss_flags = SS_DISABLE;
    }

    /** Data type for holding signal sets in a 32-bit specimen. */
    typedef uint64_t sigset_32;

    /** Kernel sigcontext pushed onto signal handler stack. */
    struct sigcontext_32 {
        uint16_t gs, __gsh;
        uint16_t fs, __fsh;
        uint16_t es, __esh;
        uint16_t ds, __dsh;
        uint32_t di;
        uint32_t si;
        uint32_t bp;
        uint32_t sp;
        uint32_t bx;
        uint32_t dx;
        uint32_t cx;
        uint32_t ax;
        uint32_t trapno;
        uint32_t err;
        uint32_t ip;
        uint16_t cs, __csh;
        uint32_t flags;
        uint32_t sp_at_signal;
        uint32_t ss, __ssh;
        uint32_t fpstate_ptr;           /* zero when no FPU/extended context */
        uint32_t oldmask;
        uint32_t cr2;
    } __attribute__((packed));

    struct siginfo_32 {
        int32_t si_signo;
        int32_t si_errno;
        int32_t si_code;
        union {
            int32_t pad[28];
            struct {                    /* kill() */
                int32_t pid;            /* sender's pid */
                uint32_t uid;           /* sender's uid */
            } kill;
            struct {                    /* POSIX.1b timers */
                int32_t tid;            /* timer ID */
                int32_t overrun;        /* overrun count */
                //int8_t pad[0];
                sigval_t sigval;        /* same as below */
                int32_t sys_private;    /* not to be passed to user */
            } timer;
            struct {                    /* POSIX.1b signals */
                int32_t pid;            /* sender's pid */
                uint32_t uid;           /* sender's uid */
                uint32_t sigval;        /* integer or address of ? */
            } rt;
            struct {                    /* SIGCHLD */
                int32_t pid;            /* which child */
                uint32_t uid;           /* sender's uid */
                uint32_t status;        /* exit code */
                int32_t utime;
                int32_t stime;
            } sigchld;
            struct {                    /* SIGILL, SIGFPE, SIGSEGV, SIGBUS */
                uint32_t addr;          /* faulting insn/memory ref */
                uint16_t addr_lsb;      /* LSB of the reported address */
            } sigfault;
            struct {                    /* SIGPOLL */
                int32_t band;           /* POLL_IN, POLL_OUT, POLL_MSG */
                int32_t fd;
            } sigpoll;
        };
    } __attribute__((packed));

    /** Kernel struct ucontext from linux/include/asm-generic/ucontext.h */
    struct ucontext_32 {
        uint32_t uc_flags;              /* 0 unless cpu_has_xsave (see Linux ia32_setup_rt_frame()) */
        uint32_t uc_link_va;            /* ptr to another ucontext_32 */
        stack_32 uc_stack;
        sigcontext_32 uc_mcontext;
        sigset_32 uc_sigmask;
    } __attribute((packed));

    /** Signal handling stack frame */
    struct sigframe_32 {
        uint32_t pretcode;              /* signal handler's return address */
        int32_t signo;                  /* signal number */
        sigcontext_32 sc;               /* signal hardware context */
        //fpstate_32 fpstate_unused;      /* no longer used by kernel, but still allocated on stack */
        uint32_t extramask;             /* blocked real-time signals */
        uint8_t retcode[8];             /* x86 code to call sigreturn; unused, but GDB magic number */
        /* fpstate follows here */
    } __attribute__((packed));

    /** Signal handling extended stack frame; used if sigaction.sa_flags has SA_SIGINFO bit set */
    struct rt_sigframe_32 {
        uint32_t pretcode;              /* signal handler's return address */
        int32_t signo;                  /* signal number */
        uint32_t pinfo;                 /* address of "info" member on stack */
        uint32_t puc;                   /* address of "uc" member on stack */
        siginfo_32 info;                /* detailed info about the signal */
        ucontext_32 uc;                 /* user context struct */
        uint8_t retcode[8];             /* x86 code to call sigreturn; unused, but GDB magic number */
    } __attribute__((packed));

    /** Signal to use when notifying a thread that a signal has been added to its queue. */
    static const int SIG_WAKEUP;

    /** Class methods to create signal information objects.  The names (after the "mk_" prefix) correspond to the union member
     *  names in the siginfo_32 struct.
     *
     *  @{ */
    static siginfo_32 mk_kill(int signo, int code);
    static siginfo_32 mk_sigfault(int signo, int code, uint32_t addr);
    static siginfo_32 mk_rt(int signo, int code);
    static siginfo_32 mk(const siginfo_t*);
    /** @} */

    /** Returns a signal set having only the specified signal. Note: this cannot be named "sigmask" since that's a macro in
     *  some versions of "signal.h".
     *
     *  Thread safety: This method is thread safe. */
    static sigset_32 mask_of(int signo);

    /** Obtain the stack pointer for a signal handler stack frame.
     *
     *  Thread safety: This method is thread safe. */
    uint32_t get_sigframe(const sigaction_32 *sa, size_t frame_size, uint32_t sp);

    /** Fill in a sigcontext_32 struct.
     *
     *  Thread safety: This method is thread safe. */
    static void setup_sigcontext(sigcontext_32 *sc, const pt_regs_32 &regs, sigset_32 mask);

    /** Initialize registers from signal context.  Not all FLAG bits are restored to their original value, which is why the set
     *  of current flags must be passed as an argument.
     *
     *  Thread safety: This method is thread safe. */
    static void restore_sigcontext(const sigcontext_32&, uint32_t cur_flags, pt_regs_32*);

    /** Adjust the signal mask.  If @p in is non-null, then adjust the signal mask according to @p how: 0 means add signals
     *  from @p in into the mask; 1 means remove signals in @p in from the mask; 2 means set the mask so it's equal to @p
     *  in. If @p out is non-null, then the original mask is returned there.  Returns zero on success, -EINVAL on error.
     *
     *  Thread safety: This method is thread safe. */
    int sigprocmask(int how, const sigset_32 *in, sigset_32 *out);

    /** Obtains the set of pending signals. These are the signals that have arrived but have not yet been processed, probably
     *  because they are set in the current sigprocmask. Returns zero on success, negative on error.  Although real-time
     *  signals are not kept up-to-date in the "pending" bit vector, they will be returned by this function by scanning the
     *  queue of real-time signals waiting to be delivered.
     *
     *  Thread safety: This method is thread safe. */
    int sigpending(sigset_32 *result) const;

    /** Block until a signal arrives.  Temporarily replaces the signal mask with the specified mask, then blocks until the
     *  delivery of a signal whose action is to invoke a signal handler or terminate the specimen. On success, returns the
     *  number of the signal that caused this function to return; returns negative on failure. (Note that the return value is
     *  not the same as the real sigsuspend.)
     *
     *  FIXME: This doesn't have quite the same semantics as the real sigsuspend(): the signal handler is invoked after this
     *  method returns and after the original signal mask has been restored. [RPM 2011-03-08]
     *
     *  Thread safety: This method is thread safe. */
    int sigsuspend(const sigset_32 *new_mask, RSIM_Thread*);

    /** Determines whether an address is on the signal stack.  Returns true if the specified address is on the signal stack,
     *  false otherwise.
     *
     *  Thread safety: This method is thread safe. */
    bool on_signal_stack(uint32_t va) const;

    /** Defines a new alternate signal stack and/or retrieve the state of an existing alternate signal stack.  This method is
     *  similar in behavior to the real sigaltstack() function, except the return value is zero on success or a negative error
     *  number on failure.  Each thread has its own alternate signal stack property.
     *
     *  It is not permissible to change the signal stack while it's being used. The @p sp argument is used only for checking
     *  whether the stack is currently in use.
     *
     *  Thread safety: This method is thread safe. */
    int sigaltstack(const stack_32 *in, stack_32 *out, uint32_t sp);
    
    /** Clear all pending signals.
     *
     *  Thread safety: This method is thread safe. */
    void clear_all_pending();

    /** Generates a signal. If the signal is a real-time signal, it will be appended to the queue of real-time signals,
     *  otherwise classic signals simply set a bit in the "pending" set.  The supplied signal information (@p siginfo) will be
     *  saved and provided on the eventual signal handlers stack frame if that handler's SA_SIGINFO flag is set.
     *
     *  If a signal's action is SIG_IGN, then the signal is not added to the queue or the pending set (it's just discarded).
     *  The specified process is used to determine if the signal is to be ignored.  Tracing is output to the specified
     *  RTS_Message, which is normally the signalling thread's TRACE_SIGNAL facility.
     *
     *  Returns zero on success, negative on failure.  The only recoverable failure that's supported is the generation of a
     *  real-time signal which causes the signal queue to be overflowed, returning -ENOBUFS.
     *
     *  Thread safety: This method is thread safe. */
    int generate(const siginfo_32 &siginfo, RSIM_Process*, RTS_Message*);

    /** Removes one unmasked signal from the set of pending signals.  Returns a signal number, or negative on failure.  If no
     *  signals are pending which are not masked, then returns zero.  If a mask is specified as an argument, then that mask is
     *  used instead of the current signal mask.
     *
     *  Thread safety: This method is thread safe. */
    int dequeue(siginfo_32 *info/*out*/, const sigset_32 *mask=NULL);

private:
    static const size_t QUEUE_SIZE = 20;
    static const int FIRST_RT = 32;     /**< Lowest numbered real-time signal; do not use SIGRTMIN. */

    mutable RTS_mutex_t mutex;          /**< Protects all members of this struct. */
    sigset_32 mask;                     /**< Masked signals. Bit N is set if signal N+1 is masked. */
    stack_32 stack;                     /**< Possible alternative stack to using during signal handling. */
    siginfo_32 queue[QUEUE_SIZE];       /**< Queue of pending real-time signals. */
    size_t queue_head;                  /**< Head of circular "queue"; points to oldest item in queue. */
    size_t queue_tail;                  /**< Tail of circular "queue"; points one past youngest item; head==tail implies empty. */
    sigset_32 pending;                  /**< Bit N is set if signal N+1 is pending; excludes real-time signals. */
    siginfo_32 pending_info[FIRST_RT+1];/**< Info for pending classic signals; rt info is in "queue". Indexed by signo. */
    bool reprocess;                     /**< Set to true if we might need to deliver signals (e.g., signal_mask changed). */
};

#endif
