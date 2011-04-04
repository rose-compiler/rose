#ifndef ROSE_RSIM_Callbacks_H
#define ROSE_RSIM_Callbacks_H


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
 *  Callbacks are called in forward or reverse order depending on the type of list. See the various "call_*" methods for
 *  details.  Any callback is allowed to modify the lists in the RSIM_Callbacks object, and these changes don't affect which
 *  callbacks are made nor the order in which they're made until the next invocation of a "call_*" method.
 *
 *  Callbacks may be made from multiple threads concurrently, so all callbacks should be thread safe.  Generally speaking, one
 *  should not assume that any particular mutexes or read-write locks are held when the callback is invoked.  All of the
 *  RSIM_Callbacks methods are thread safe.
 *
 *  \section Example1 Example:  Disassembling a dynamically linked binary
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
 *  \code
 *  struct DisassembleAtOep: public RSIM_Callbacks::InsnCallback {
 *      // Share a single callback in simulator, process, and all threads
 *      virtual DisassembleAtOep *clone() { return this; }
 *
 *      // The actual callback
 *      virtual bool operator()(RSIM_Thread *thread, SgAsmInstruction *insn, bool prev) {
 *          if (thread->get_process()->get_ep_orig_va() == insn->get_address()) {
 *              // This thread is at the OEP.  Call the thread-safe disassembler.
 *              SgAsmBlock *block = thread->get_process()->disassemble();
 *              // Output disassembled instructions, functions, etc.  See disassemble.C
 *              // in tests/roseTests/binaryTests for more sophisticated examples of
 *              // displaying instructions and other information using AsmUnparser.
 *              AsmUnparser().unparse(std::cout, block);
 *              // Remove this callback from this thread. Also, by removing it from the
 *              // process we prevent subsequently created threads from incuring the
 *              // runtime overhead of invoking this callback on every instruction.
 *              thread->get_callbacks().remove_pre_insn(this);
 *              thread->get_process()->get_callbacks().remove_pre_insn(this);
 *          }
 *          return prev;
 *      }
 *  };
 *
 *  // Register the callback to be called before every instruction.  Registering it
 *  // for the entire simulator will cause it to also be registered for the specimen's
 *  // process and threads when they are created later.
 *  RSIM_Simulator simulator;
 *  simulator.get_callbacks().add_pre_insn(new DisassembleAtOep);
 *  \endcode
 *
 *  Continue by configuring the simulator, loading the specimen executable, activating
 *  the simulator, running the simulator, etc.  An example is provided on the RSIM
 *  main page (\ref index).
 */
class RSIM_Callbacks {
    /**************************************************************************************************************************
     *                                  Types
     **************************************************************************************************************************/
public:

    /** Base class for all simulator callbacks. */
    class Callback {
    public:
        virtual ~Callback() {}

        /** Allocate a copy of this object. */
        virtual Callback *clone() = 0;
    };

    /** Instruction-related callbacks. */
    class InsnCallback: public Callback {
    public:
        /** Method invoked on an instruction. The @p prev argument is the return value from the previous callback when multiple
         * callbacks are registered. See also, RSIM_Callbacks::add_pre_insn() and RSIM_Callbacks::add_post_insn(). */
        virtual bool operator()(RSIM_Thread*, SgAsmInstruction*, bool prev) = 0;
    };

    /** Thread-related callbacks. */
    class ThreadCallback: public Callback {
    public:
        virtual bool operator()(RSIM_Thread*, bool prev) = 0;
    };

    
    /**************************************************************************************************************************
     *                                  Constructors, etc.
     **************************************************************************************************************************/
private:
    void init(const RSIM_Callbacks &);

public:
    RSIM_Callbacks() {
        pthread_mutex_init(&mutex, NULL);
    }

    /** Thread-safe copy constructor. */
    RSIM_Callbacks(const RSIM_Callbacks &other) {
        pthread_mutex_init(&mutex, NULL);
        init(other);
    }

    /** Thread-safe assignment. */
    RSIM_Callbacks& operator=(const RSIM_Callbacks &other) {
        init(other);
        return *this;
    }

    /**************************************************************************************************************************
     *                                  Registering and Unregistering
     **************************************************************************************************************************/
public:

    /** Registers a callback object that should be invoked before every simulated instruction.  Pre-instruction callbacks are
     *  invoked in the opposite order they were registered (see call_pre_insn() for details).  The specified callback object is
     *  inserted into the list without copying it.
     *
     *  Thread safety:  This method is thread safe. */
    void add_pre_insn(InsnCallback*);

    /** Unregisters a pre-instruction callback. The specified callback is removed from the list of pre-instruction callbacks.
     *  If a single callback object was registered more than once, then only the most recently registered instance is removed.
     *  This method returns true if a callback was removed.  The removed callback object is not destroyed.
     *
     *  Thread safety:  This method is thread safe. */
    bool remove_pre_insn(InsnCallback*);

    /** Unregisters all pre-instruction callbacks.
     *
     *  Thread safety:  This method is thread safe. */
    void clear_pre_insn();



    /** Registers a callback object that should be invoked after every simulated instruction.  Post-instruction callbacks are
     *  invoked in the order they were registered (see call_post_insn() for details).  The specified callback object is
     *  inserted into the list without copying it.
     *
     *  Thread safety:  This method is thread safe. */
    void add_post_insn(InsnCallback*);

    /** Unregisters a post-instruction callback.  The specified callback is removed from the list of post-instruction
     *  callbacks. If a single callback object was registered more than once, then only the most recently registered instance is
     *  removed. This method returns true if a callback was removed.  The removed callback object is not destroyed. */
    bool remove_post_insn(InsnCallback*);

    /** Unregisters all post-instruction callbacks.
     *
     *  Thread safety:  This method is thread safe. */
    void clear_post_insn();



    /** Registers a callback object that should be invoked when a thread is created.  Pre-thread callbacks are invoked in the
     *  opposite order they were registered (see call_pre_thread() for details).  The specified callback object is inserted
     *  into the list without copying it.
     *
     *  Thread safety:  This method is thread safe. */
    void add_pre_thread(ThreadCallback*);

    /** Unregisters a pre-thread callback.  The specified callback is removed from the list of thread callbacks. If a single
     *  callback object was registered more than once, then only the most recently registered instance is removed.  This method
     *  returns true if a callback was removed.  The removed callback is not destroyed.
     *
     *  Thread safety:  This method is thread safe. */
    bool remove_pre_thread(ThreadCallback*);

    /** Unregisters all pre-thread callbacks.
     *
     *  Thread safety:  This method is thread safe. */
    void clear_pre_thread();
    
    

    /** Registers a callback object that should be invoked when a thread is destroyed.  Post-thread callbacks are invoked in
     *  the order they were registered (see call_post_thread() for details).  The specified callback object is inserted into
     *  the list without copying it.
     *
     *  Thread safety:  This method is thread safe. */
    void add_post_thread(ThreadCallback*);

    /** Unregisters a post-thread callback.  The specified callback is removed from the list of thread callbacks. If a single
     *  callback object was registered more than once, then only the most recently registered instance is removed.  This method
     *  returns true if a callback was removed.  The removed callback is not destroyed.
     *
     *  Thread safety:  This method is thread safe. */
    bool remove_post_thread(ThreadCallback*);

    /** Unregisters all post-thread callbacks.
     *
     *  Thread safety:  This method is thread safe. */
    void clear_post_thread();
    
    

    /**************************************************************************************************************************
     *                                  Invoking callbacks
     **************************************************************************************************************************/
public:

    /** Invokes all the pre-instruction callbacks in the opposite order they were registered.  The @p prev argument is passed
     *  as the @p prev value of the first callback, while the return value of a callback is used as the @p prev value for the
     *  next callback.  The return value of the final callback (or the original @p prev if there are no callbacks) becomes the
     *  return value of this method and is used to determine whether the instruction should be simulated (true) or skipped
     *  (false). When skipping an instruction, it is probably necessary to adjust the thread's EIP register, or else the same
     *  instruction will be processed in the subsequent iteration of the thread's main simulation loop.
     *
     *  Thread safety:  This method is thread safe.  The callbacks may register and/or unregister themselves or other callbacks
     *  from this RSIM_Callbacks object, but those actions do not affect which callbacks are made by this invocation of
     *  call_pre_insn(). */
    bool call_pre_insn(RSIM_Thread*, SgAsmInstruction*, bool prev) const;

    /** Invokes all the post-instruction callbacks in the order they were registered.  The @p prev argument (the return value
     *  of the previous call_pre_insn() invocation) is passed to the first post-instruction callback. The @p prev argument for
     *  subsequent callbacks is the return value of the previous callback.  This method returns the return value of the last
     *  callback (or @p prev if there are no callbacks), and is ignored by the thread's main simulation loop.
     *
     *  Thread safety:  This method is thread safe.  The callbacks may register and/or unregister themselves or other callbacks
     *  from this RSIM_Callbacks object, but those actions do not affect which callbacks are made by this invocation of
     *  call_post_insn(). */
    bool call_post_insn(RSIM_Thread*, SgAsmInstruction*, bool prev) const;

    /** Invokes all the pre-thread callbacks in the opposite order they were registered. The @p prev argument is passed to the
     *  first pre-thread callback.  The @p prev argument for subsequent callbacks is the return value of the previous
     *  callback.  This method returns the return value of the last callback (or @p prev if there are no callbacks), which is
     *  normally provided to the call_post_thread() method.
     *
     *  Thread safety:  This method is thread safe.  The callbacks may register and/or unregister themselves or other callbacks
     *  from this RSIM_Callbacks object, but those actions do not affect which callbacks are made by this invocation of
     *  call_pre_thread(). */
    bool call_pre_thread(RSIM_Thread*, bool prev) const;
    
    /** Invokes all the post-thread callbacks in the order they were registered.  The @p prev argument (the return value
     *  of the previous call_pre_thread() invocation) is passed to the first post-thread callback. The @p prev argument for
     *  subsequent callbacks is the return value of the previous callback.  This method returns the return value of the last
     *  callback (or @p prev if there are no callbacks).
     *
     *  Thread safety:  This method is thread safe.  The callbacks may register and/or unregister themselves or other callbacks
     *  from this RSIM_Callbacks object, but those actions do not affect which callbacks are made by this invocation of
     *  call_post_insn(). */
    bool call_post_thread(RSIM_Thread*, bool prev) const;



    /**************************************************************************************************************************
     *                                  Data members
     **************************************************************************************************************************/
private:
    mutable pthread_mutex_t mutex;

    /* See init() if you add more vectors */
    std::vector<InsnCallback*> pre_insn;
    std::vector<InsnCallback*> post_insn;
    std::vector<ThreadCallback*> pre_thread;
    std::vector<ThreadCallback*> post_thread;
};

#endif /* ROSE_RSIM_Callbacks_H */
