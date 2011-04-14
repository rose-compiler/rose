#ifndef ROSE_RSIM_Callbacks_H
#define ROSE_RSIM_Callbacks_H

#include "callbacks.h" /* for ROSE_Callbacks namespace */

/** Set of callbacks.  Callbacks are user-supplied objects whose operator() is invoked at particular points during a
 *  simulation.  The set of callbacks is organized into lists and all callbacks present on a list are invoked at the desired
 *  time.  The arguments for the callback depend on the kind of callback, but always include a boolean value, which is the
 *  return value of the previous callback on the list (or related list), or true for the first callback.
 *
 *  Callbacks are associated with threads, processes, or the simulator as a whole.  When a new thread is created its callbacks
 *  are initialized from the process; and when a new process is created its callbacks are initialized from the simulator.  When
 *  a callback list is copied, each callback's clone() method is invoked to generate the new copy.  Since threads and processes
 *  are never deleted (at least not until the simulator is deleted), their callback lists are never deleted.  Removal of a
 *  callback from a list also does not delete the callback. Therefore it's safe for the clone() method to be a no-op, simply
 *  returning a pointer to the original object.
 *
 *  Callbacks are called in the order they were added to the list. See the various "call_*" methods for details.  Any callback
 *  is allowed to modify the lists in the RSIM_Callbacks object, and these changes don't affect which callbacks are made nor
 *  the order in which they're made until the next invocation of a "call_*" method.
 *
 *  Callbacks may be made from multiple threads concurrently, so all callbacks should be thread safe.  Generally speaking, one
 *  should not assume that any particular mutexes or read-write locks are held when the callback is invoked.  All of the
 *  RSIM_Callbacks methods are thread safe.
 *
 *  @section Example1 Example:  Disassembling a dynamically linked binary
 *
 *  Binary executables are often dynamically linked, wherein the main binary contains only stubs for dynamic functions and
 *  those functions' text and data are loaded into process memory and linked at run time.  Since most disassemblers don't
 *  perform these steps when disassembling, they don't disassemble the dynamic libraries.  In fact, under Linux, the dynamic
 *  linker is itself a dynamic library which is loaded into the process address space by the kernel, which then starts
 *  execution at the linker's entry point.  Since the simulator makes the original entry point (OEP) available to the user, we
 *  can simply allow the specimen executable to be simulated until we reach the OEP, and then invoke the ROSE disassembler on
 *  the specimen memory map.
 *
 *  What follows is a very simplistic example of disassembling memory when the OEP is reached.  The example callback is invoked
 *  before every instruction.  If the instruction address matches the OEP then the specimen's memory is disassembled (in a
 *  thread-safe manner) and the assembly is unparsed to the standard output stream.  The callback is then removed from the
 *  calling thread's callbacks (although it might still be called from other threads that try to execute at the OEP).
 *
 *  @code
 *  struct DisassembleAtOep: public RSIM_Callbacks::InsnCallback {
 *      // Share a single callback among the simulator, the process, and all threads.
 *      virtual DisassembleAtOep *clone() { return this; }
 *
 *      // The actual callback. See ROSE_Callbacks::List (in the ROSE documentation)
 *      // for why it's defined this way, particularly the Args argument.  The purpose
 *      // of the Boolean arg is described in ROSE_Callbacks::List::apply().
 *      virtual bool operator()(bool prev, const Args &args) {
 *          RSIM_Process *process = args.thread->get_process();
 *          if (process->get_ep_orig_va() == args.insn->get_address()) {
 *              // This thread is at the OEP.  Call the thread-safe disassembler.
 *              SgAsmBlock *block = process->disassemble();
 *
 *              // Output disassembled instructions, functions, etc.  See disassemble.C
 *              // in tests/roseTests/binaryTests for more sophisticated examples of
 *              // displaying instructions and other information using AsmUnparser.
 *              AsmUnparser().unparse(std::cout, block);
 *
 *              // Remove this callback from this thread. Also, by removing it from the
 *              // process we prevent subsequently created threads from incuring the
 *              // runtime overhead of invoking this callback on every instruction. Removal
 *              // is entirely optional.
 *              args.thread->get_callbacks().remove_insn_callback(RSIM_Callbacks::BEFORE, this);
 *              process->get_callbacks().remove_insn_callback(RSIM_Callbacks::BEFORE, this);
 *          }
 *          return prev;
 *      }
 *  };
 *
 *  // Register the callback to be called before every instruction.  Registering it
 *  // for the entire simulator will cause it to also be registered for the specimen's
 *  // process and threads when they are created later.
 *  RSIM_Simulator simulator;
 *  simulator.get_callbacks().add_insn_callback(RSIM_Callbacks::BEFORE, new DisassembleAtOep);
 *  @endcode
 *
 *  Continue by configuring the simulator, loading the specimen executable, activating the simulator, running the simulator,
 *  etc.  An example is provided on the RSIM main page (\ref index).
 *
 *  @section Example2 Example: Using the disassembler to trace functions
 *
 *  A side effect of calling RSIM_Process::disassemble() is that the resulting AST contains instruction nodes
 *  (SgAsmInstruction) whose grandparents are function nodes (SgAsmFunctionDeclaration).  We can use this fact to print the
 *  name of the function in which we're currently executing.
 *
 *  @code
 *  class ShowFunction: public RSIM_Callbacks::InsnCallback {
 *  public:
 *      // Share a single callback among the simulator, the process, and all threads.
 *      virtual ShowFunction *clone() { return this; }
 *
 *      // The actual callback.
 *      virtual bool operator()(bool prev, const Args &args) {
 *          SgAsmBlock *basic_block = isSgAsmBlock(args.insn->get_parent());
 *          SgAsmFunctionDeclaration *func = basic_block ? isSgAsmFunctionDeclaration(basic_block->get_parent()) : NULL;
 *          if (func && func->get_name()!=name) {
 *              name = func->get_name();
 *              args.thread->tracing(TRACE_MISC)->mesg("in function \"%s\"", name.c_str());
 *          }
 *          return prev;
 *      }
 *
 *  private:
 *      std::string name;
 *  };
 * 
 *  sim.get_callbacks().add_insn_callback(RSIM_Callbacks::BEFORE, new ShowFunction);
 *  @endcode
 *
 *  The output will be something along the following lines.  The "28129:1" means the main thread of process 28129. The floating
 *  point number which follows is the time in seconds since the start of the simulation.  The hexadecimal number is the address
 *  of the instruction that's about to be executed followed by an instruction counter in square brackets.  Our "in function"
 *  output appears, in this example, with system call tracing that contains the name of the system call, the system call number
 *  from <asm/unistd_32.h>, the arguments, and the return value.
 *
 *  @code
 *  28129:1 31.292 0x08048132[1]:       in function "_start"
 *  28129:1 31.292 0x0804bea0[13]:      in function "__libc_start_main"
 *  28129:1 31.345 0x08061db3[235]:     in function "_dl_aux_init"
 *  28129:1 31.345 0x0804bee7[250]:     in function "__libc_start_main"
 *  28129:1 31.345 0x0805e7c0[256]:     in function "__uname"
 *  28129:1 31.345 0x0805e7cd[260]:     uname[122](0xbfffddb6) = 0
 *  28129:1 31.345 0x0804bf03[264]:     in function "__libc_start_main"
 *  28129:1 31.346 0x0804b890[338]:     in function "__pthread_initialize_minimal_internal"
 *  28129:1 31.346 0x0804c375[348]:     in function "__libc_setup_tls"
 *  28129:1 31.346 0x0805f5c8[376]:     in function "__sbrk"
 *  28129:1 31.346 0x080871f0[387]:     in function "brk"
 *  28129:1 31.346 0x080871fe[393]:     brk[45](0) = 0x080d5000
 *  28129:1 31.346 0x0805f605[401]:     in function "__sbrk"
 *  28129:1 31.346 0x080871f0[409]:     in function "brk"
 *  28129:1 31.346 0x080871fe[415]:     brk[45](0x080d5c80) = 0x080d5c80
 *  28129:1 31.346 0x0805f628[423]:     in function "__sbrk"
 *  28129:1 31.347 0x0804c40d[431]:     in function "__libc_setup_tls"
 *  28129:1 31.347 0x0805e070[455]:     in function "memcpy"
 *  @endcode
 */
class RSIM_Callbacks {
    /**************************************************************************************************************************
     *                                  Types
     **************************************************************************************************************************/
public:

    enum When { BEFORE, AFTER };

    /** Base class for all simulator callbacks. */
    class Callback {
    public:
        virtual ~Callback() {}

        /** Allocate a copy of this object. */
        virtual Callback *clone() = 0;
    };



    /**************************************************************************************************************************
     *                                  Constructors, etc.
     **************************************************************************************************************************/
private:
    void init(const RSIM_Callbacks &);

public:
    RSIM_Callbacks() {}

    /** Thread-safe copy constructor. */
    RSIM_Callbacks(const RSIM_Callbacks &other) {
        init(other);
    }

    /** Thread-safe assignment. */
    RSIM_Callbacks& operator=(const RSIM_Callbacks &other) {
        init(other);
        return *this;
    }

    /**************************************************************************************************************************
     *                                  Instruction callbacks
     **************************************************************************************************************************/
public:

    /** Instruction callbacks invoked on every instruction. */
    class InsnCallback: public Callback {
    public:
        struct Args {
            Args(RSIM_Thread *thread, SgAsmInstruction *insn)
                : thread(thread), insn(insn) {}
            RSIM_Thread *thread;
            SgAsmInstruction *insn;
        };
        virtual bool operator()(bool prev, const Args&) = 0;
    };

    /** Registers an instruction callback.  Instruction callbacks are invoked before or after (depending on @p when) every
     *  simulated instruction.  The specified callback object is inserted into the list without copying it. See
     *  call_insn_callbacks() for details about how these callbacks are invoked.
     *
     *  Thread safety:  This method is thread safe. */
    void add_insn_callback(When, InsnCallback*);

    /** Unregisters an instruction callback.  The most recently registered instance of the specified callback (if any) is
     *  removed from the pre- or post-instruction callback list, depending on the value of @p when).  The removed callback
     *  object is not destroyed.  Returns true if a callback was removed, false if not.
     *
     *  Thread safety:  This method is thread safe. */
    bool remove_insn_callback(When, InsnCallback*);


    /** Removes all instruction callbacks.  The pre- or post-instruction callbacks are removed, depending on the value of @p
     * when. None of the removed callbacks are destroyed.
     *
     *  Thread safety:  This method is thread safe. */
    void clear_insn_callbacks(When);

    /** Invokes all the instruction callbacks.  The pre- or post-instruction callbacks (depending on the value of @p when) are
     *  invoked in the order they were registered.  The specified @p prev value is passed to the first callback as its @p prev
     *  argument; subsequent callbacks' @p prev argument is the return value of the previous callback; the return value of the
     *  final callback becomes the return value of this method.  However, if no callbacks are invoked (because the list is
     *  empty) then this method's return value is the specified @p prev value.  The @p thread and @p insn are passed to each of
     *  the callbacks.
     *
     *  When the simulator calls this function for pre-instruction callbacks, it does so with @p prev set.  If the return value
     *  is false, then the instruction is not simulated. When an instruction is skipped, the callback should, as a general
     *  rule, adjust the EIP register so that the same instruction is not simulated ad nauseam.  The post-instruction callbacks
     *  are invoked regardless of whether the instruction was simulated, and the initial @p prev value for these callbacks is
     *  the return value from the last pre-instruction callback (or true if there were none).
     *
     *  Thread safety:  This method is thread safe.  The callbacks may register and/or unregister themselves or other callbacks
     *  from this RSIM_Callbacks object, but those actions do not affect which callbacks are made by this invocation of
     *  call_insn_callbacks(). */
    bool call_insn_callbacks(When, RSIM_Thread *thread, SgAsmInstruction *insn, bool prev);



    /**************************************************************************************************************************
     *                                  System Call Callbacks
     **************************************************************************************************************************/
public:

    /** System call callbacks invoked on every system call. */
    class SyscallCallback: public Callback {
    public:
        struct Args {
            Args(RSIM_Thread *thread, int callno)
                : thread(thread), callno(callno) {}
            RSIM_Thread *thread;
            int callno;
        };
        virtual bool operator()(bool prev, const Args&) = 0;
    };
    
    /** Registers a system call callback.  System call callbacks are invoked before or after (depending on @p when) every
     *  simulated system call.  The specified callback object is inserted into the list without copying it. See
     *  call_syscall_callbacks() for details about how these callbacks are invoked.
     *
     *  Thread safety:  This method is thread safe. */
    void add_syscall_callback(When, SyscallCallback*);

    /** Unregisters a system call callback.  The most recently registered instance of the specified callback (if any) is
     *  removed from the pre- or post-syscall callback list, depending on the value of @p when).  The removed callback
     *  object is not destroyed.  Returns true if a callback was removed, false if not.
     *
     *  Thread safety:  This method is thread safe. */
    bool remove_syscall_callback(When, SyscallCallback*);


    /** Removes all system call callbacks.  The pre- or post-syscall callbacks are removed, depending on the value of @p
     * when. None of the removed callbacks are destroyed.
     *
     *  Thread safety:  This method is thread safe. */
    void clear_syscall_callbacks(When);

    /** Invokes all the system call callbacks.  The pre- or post-syscall callbacks (depending on the value of @p when) are
     *  invoked in the order they were registered.  The specified @p prev value is passed to the first callback as its @p prev
     *  argument; subsequent callbacks' @p prev argument is the return value of the previous callback; the return value of the
     *  final callback becomes the return value of this method.  However, if no callbacks are invoked (because the list is
     *  empty) then this method's return value is the specified @p prev value.  The @p thread and @p callno are passed to each
     *  of the callbacks.
     *
     *  When the simulator calls this function for pre-syscall callbacks, it does so with @p prev set.  If the return value is
     *  false, then the system call is not simulated. When a system call is skipped, the callback should, as a general rule,
     *  adjust the EAX register to indicate the syscall return value (errors are indicated, at least for Linux system calls, as
     *  negative integers corresponding to the values in <errno.h>.  The post-syscall callbacks are invoked regardless of
     *  whether the system call was simulated, and the initial @p prev value for these callbacks is the return value from the
     *  last pre-syscall callback (or true if there were none).
     *
     *  Thread safety:  This method is thread safe.  The callbacks may register and/or unregister themselves or other callbacks
     *  from this RSIM_Callbacks object, but those actions do not affect which callbacks are made by this invocation of
     *  call_syscall_callbacks(). */
    bool call_syscall_callbacks(When, RSIM_Thread *thread, int callno, bool prev);



    /**************************************************************************************************************************
     *                                  Thread Callbacks
     **************************************************************************************************************************/
public:

    /** Thread callbacks invoked on every thread. */
    class ThreadCallback: public Callback {
    public:
        struct Args {
            Args(RSIM_Thread *thread)
                : thread(thread) {}
            RSIM_Thread *thread;
        };
        virtual bool operator()(bool prev, const Args&) = 0;
    };

    /** Registers a thread callback.  Thread callbacks are invoked before a new thread is created or after a thread exits
     *  (depending on @p when).  The specified callback object is inserted into the list without copying it. See
     *  call_thread_callbacks() for details about how these callbacks are invoked.
     *
     *  Thread safety:  This method is thread safe. */
    void add_thread_callback(When, ThreadCallback*);

    /** Unregisters a thread callback.  The most recently registered instance of the specified callback (if any) is removed
     *  from the pre- or post-thread callback list, depending on the value of @p when).  The removed callback object is not
     *  destroyed.  Returns true if a callback was removed, false if not.
     *
     *  Thread safety:  This method is thread safe. */
    bool remove_thread_callback(When, ThreadCallback*);


    /** Removes all thread callbacks.  The pre- or post-thread callbacks are removed, depending on the value of @p when. None
     * of the removed callbacks are destroyed.
     *
     *  Thread safety:  This method is thread safe. */
    void clear_thread_callbacks(When);

    /** Invokes all the thread callbacks.  The pre- or post-thread callbacks (depending on the value of @p when) are invoked in
     *  the order they were registered.  The specified @p prev value is passed to the first callback as its @p prev argument;
     *  subsequent callbacks' @p prev argument is the return value of the previous callback; the return value of the final
     *  callback becomes the return value of this method.  However, if no callbacks are invoked (because the list is empty)
     *  then this method's return value is the specified @p prev value.  The @p thread is passed to each of the callbacks.
     *
     *  Thread safety:  This method is thread safe.  The callbacks may register and/or unregister themselves or other callbacks
     *  from this RSIM_Callbacks object, but those actions do not affect which callbacks are made by this invocation of
     *  call_thread_callbacks(). */
    bool call_thread_callbacks(When, RSIM_Thread *thread, bool prev);



    /**************************************************************************************************************************
     *                                  Data members
     **************************************************************************************************************************/
private:
    /* See init() if you add more vectors */
    ROSE_Callbacks::List<InsnCallback> insn_pre;
    ROSE_Callbacks::List<InsnCallback> insn_post;

    ROSE_Callbacks::List<SyscallCallback> syscall_pre;
    ROSE_Callbacks::List<SyscallCallback> syscall_post;

    ROSE_Callbacks::List<ThreadCallback> thread_pre;
    ROSE_Callbacks::List<ThreadCallback> thread_post;
};

#endif /* ROSE_RSIM_Callbacks_H */
