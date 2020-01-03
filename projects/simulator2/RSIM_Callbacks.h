#ifndef ROSE_RSIM_Callbacks_H
#define ROSE_RSIM_Callbacks_H

#include "RSIM_SignalHandling.h"

#include "callbacks.h" /* for Rose::Callbacks namespace */

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
 *      // The actual callback. See Rose::Callbacks::List (in the ROSE documentation)
 *      // for why it's defined this way, particularly the Args argument.  The purpose
 *      // of the Boolean arg is described in Rose::Callbacks::List::apply().
 *      virtual bool operator()(bool prev, const Args &args) {
 *          RSIM_Process *process = args.thread->get_process();
 *          if (process->get_ep_orig_va() == args.insn->get_address()) {
 *              // This thread is at the OEP.  Call the thread-safe disassembler.
 *              SgAsmBlock *block = process->disassemble();
 *
 *              // Output disassembled instructions, functions, etc.  See disassemble.C
 *              // in tests/nonsmoke/functional/roseTests/binaryTests for more sophisticated examples of
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
 *  (SgAsmInstruction) whose grandparents are function nodes (SgAsmFunction).  We can use this fact to print the
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
 *          SgAsmFunction *func = basic_block ?
 *                                SageInterface::getEnclosingNode<SgAsmFunction>(basic_block) :
 *                                NULL;
 *          if (func && func->get_name()!=name) {
 *              name = func->get_name();
 *              args.thread->tracing(TRACE_MISC) <<"in function \" <<name <<"\"\n";
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
 *  @verbatim
 28129:1 31.292 0x08048132[1]:       in function "_start"
 28129:1 31.292 0x0804bea0[13]:      in function "__libc_start_main"
 28129:1 31.345 0x08061db3[235]:     in function "_dl_aux_init"
 28129:1 31.345 0x0804bee7[250]:     in function "__libc_start_main"
 28129:1 31.345 0x0805e7c0[256]:     in function "__uname"
 28129:1 31.345 0x0805e7cd[260]:     uname[122](0xbfffddb6) = 0
 28129:1 31.345 0x0804bf03[264]:     in function "__libc_start_main"
 28129:1 31.346 0x0804b890[338]:     in function "__pthread_initialize_minimal_internal"
 28129:1 31.346 0x0804c375[348]:     in function "__libc_setup_tls"
 28129:1 31.346 0x0805f5c8[376]:     in function "__sbrk"
 28129:1 31.346 0x080871f0[387]:     in function "brk"
 28129:1 31.346 0x080871fe[393]:     brk[45](0) = 0x080d5000
 28129:1 31.346 0x0805f605[401]:     in function "__sbrk"
 28129:1 31.346 0x080871f0[409]:     in function "brk"
 28129:1 31.346 0x080871fe[415]:     brk[45](0x080d5c80) = 0x080d5c80
 28129:1 31.346 0x0805f628[423]:     in function "__sbrk"
 28129:1 31.347 0x0804c40d[431]:     in function "__libc_setup_tls"
 28129:1 31.347 0x0805e070[455]:     in function "memcpy" @endverbatim
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

        /** Possibly allocate a copy of this object. */
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

        /** Arguments passed to instruction callbacks. */
        struct Args {
            Args(RSIM_Thread *thread, SgAsmInstruction *insn)
                : thread(thread), insn(insn) {}
            RSIM_Thread *thread;                /**< The thread on which the instruction executes. */
            SgAsmInstruction *insn;             /**< The instruction executing. */
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
     *  If a pre-instruction callback changes the value of the EIP register so that it is different than the instruction's
     *  address, the instruction dispatch loop will re-fetch the instruction (at the new address) without first executing any
     *  of the post-instruction callbacks for the original instruction.  This repeats until the post-callback EIP matches the
     *  instruction, regardless of whether the final pre-instruction callback returned true or false.
     *
     *  Thread safety:  This method is thread safe.  The callbacks may register and/or unregister themselves or other callbacks
     *  from this RSIM_Callbacks object, but those actions do not affect which callbacks are made by this invocation of
     *  call_insn_callbacks(). */
    bool call_insn_callbacks(When, RSIM_Thread *thread, SgAsmInstruction *insn, bool prev);



    /**************************************************************************************************************************
     *                                  Memory Callbacks
     **************************************************************************************************************************/
public:

    /** Memory callbacks invoked on every memory access. */
    class MemoryCallback: public Callback {
    public:

        /** Arguments passed to memory callbacks.  Note that the @p how argument is always either read or write depending on
         *  the operation being performed, but the the @p req_perms argument might have other bits set.  For instance, when
         *  fetching instructions, @p how will be read while @p req_perms will be execute.
         *
         *  If a callback chooses to transfer data itself, it should use the @p buffer (the buffer contains the data for the
         *  write callbacks, and should be filled in with results for read callbacks).  The buffer is allocated before the
         *  callbacks are invoked, and is guaranteed to be at lease @p nbytes in length.  If the final callback returns true
         *  (see call_memory_callbacks() for details) then the simulator transfers data as usual, otherwise it assumes that one
         *  of the callbacks has transferred the data and the simulator uses @p nbytes_xfer as the return value of the memory
         *  operation. */
        struct Args {
            Args(RSIM_Process *process, unsigned how, unsigned req_perms, rose_addr_t va, size_t nbytes,
                 void *buffer, size_t *nbytes_xfer/*out*/)
                : process(process), how(how), req_perms(req_perms), va(va), nbytes(nbytes),
                  buffer(buffer), nbytes_xfer(nbytes_xfer) {}
            RSIM_Process *process;              /**< The process whose memory is accessed. */
            unsigned how;                       /**< How memory is being access. */
            unsigned req_perms;                 /**< Memory required permissions for access. */
            rose_addr_t va;                     /**< Virtual address for beginning of memory access. */
            size_t nbytes;                      /**< Size of memory access. */
            void *buffer;                       /**< Buffer to be filled by read callbacks; contains data for writes. */
            size_t *nbytes_xfer;                /**< Amount of data transferred. */
        };
        virtual bool operator()(bool prev, const Args&) = 0;
    };
        
    /** Registers a memory callback.  Memory callbacks are invoked before or after (depending on @p when) every
     *  memory access.  The specified callback object is inserted into the list without copying it. See
     *  call_memory_callbacks() for details about how these callbacks are invoked.
     *
     *  Thread safety:  This method is thread safe. */
    void add_memory_callback(When, MemoryCallback*);

    /** Unregisters a memory callback.  The most recently registered instance of the specified callback (if any) is
     *  removed from the pre- or post-memory callback list, depending on the value of @p when).  The removed callback
     *  object is not destroyed.  Returns true if a callback was removed, false if not.
     *
     *  Thread safety:  This method is thread safe. */
    bool remove_memory_callback(When, MemoryCallback*);


    /** Removes all memory callbacks.  The pre- or post-memory callbacks are removed, depending on the value of @p
     * when. None of the removed callbacks are destroyed.
     *
     *  Thread safety:  This method is thread safe. */
    void clear_memory_callbacks(When);

    /** Invokes all the memory callbacks.  The pre- or post-memory callbacks (depending on the value of @p when) are
     *  invoked in the order they were registered.  The specified @p prev value is passed to the first callback as its @p prev
     *  argument; subsequent callbacks' @p prev argument is the return value of the previous callback; the return value of the
     *  final callback becomes the return value of this method.  However, if no callbacks are invoked (because the list is
     *  empty) then this method's return value is the specified @p prev value.  The other arguments are passed to each of
     *  the callbacks.
     *
     *  When the simulator calls this function for pre-memory callbacks, it does so with @p prev set.  If the return value is
     *  false (i.e., the final callback returned false), then the simulator does not access specimen memory, but rather uses
     *  the value saved into the buffer (if it was a read operation) and returns the number of transferred bytes.  If the
     *  function returns true then the specimen memory operation proceeds as usual.  Memory write callbacks should not modify
     *  the buffer.
     *
     *  The post-memory callbacks are invoked regardless of whether the memory was accessed, and the initial @p prev value for
     *  these callbacks is the return value from the last pre-memory callback (or true if there were none).  The @p buffer and
     *  @p nbytes_xfer contain the data and result of the memory operation and can be modified by post-memory callbacks if
     *  desired (new values will be used regardless of the call_memory_callbacks() return value).  Memory write callbacks
     *  should not modify the buffer, but may adjust the nbytes_xfer field.
     *
     *  The return value of the callbacks is ignored when the simulator is fetching instructions because the disassembler reads
     *  directly from process memory without going through the RSIM_Process memory interface.  However, we've added memory read
     *  operations (word-at-a-time) around the instruction fetching so that memory callbacks are invoked.  The @p how will be
     *  read, while the @p req_perms will be execute.
     *
     *  Thread safety:  This method is thread safe.  The callbacks may register and/or unregister themselves or other callbacks
     *  from this RSIM_Callbacks object, but those actions do not affect which callbacks are made by this invocation of
     *  call_memory_callbacks(). */
    bool call_memory_callbacks(When, RSIM_Process *process, unsigned how, unsigned req_perms,
                               rose_addr_t va, size_t nbytes, void *buffer, size_t &nbytes_xfer, bool prev);




    /**************************************************************************************************************************
     *                                  System Call Callbacks
     **************************************************************************************************************************/
public:

    /** System call callbacks invoked on every system call. */
    class SyscallCallback: public Callback {
    public:
        /** Arguments passed to system call callbacks. */
        struct Args {
            Args(RSIM_Thread *thread, int callno)
                : thread(thread), callno(callno) {}
            RSIM_Thread *thread;                /**< The thread on which the system call executes. */
            int callno;                         /**< The system call ID number. See <asm/unistd_32.h>. */
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
     *                                  Signal Callbacks
     **************************************************************************************************************************/
public:

    /** Signal callbacks.
     *
     *  These callbacks are invoked when a signal arrives at a thread or is delivered to the specimen (see the Reason
     *  enumeration for definitions).  The specified RSIM_Thread passed as an argument to the callbacks (and not necessarily
     *  corresponding to the calling thread) is the thread at which the signal has arrived or to which the signal is being
     *  delivered.  The callbacks may cancel arrival or delivery by either returning false, or by setting the info.si_signo
     *  member to zero.  The callbacks may change which signal has arrived or is delivered, or the information about the signal
     *  by modifying the @p info argument.
     *
     *  Here's an example that prints a stack trace every time a signal arrives:
     *  @code
     *  // Thread callback to generate a stack trace when a signal arrives.
     *  class SignalStackTrace: public RSIM_Callbacks::SignalCallback {
     *  public:
     *      virtual SignalStackTrace *clone() { return this; }
     *      virtual bool operator()(bool prev, const Args &args) {
     *          if (args.reason == ARRIVAL) {
     *              args.thread->get_process()->disassemble(); // optional, so stack trace has function names
     *              args.thread->report_stack_frames(args.thread->tracing(TRACE_MISC));
     *          }
     *          return prev;
     *      }
     *  };
     *
     *  int main() {
     *     RSIM_Linux32 sim;
     *     ...
     *     sim.get_callbacks().add_signal_callback(RSIM_Callbacks::AFTER, new SignalStackTrace);
     *     ...
     *  }
     *  @endcode
     *
     *  The output may look something like the following:
     *
     * @verbatim
 4474:1 28.699 0x08094233[36578]:   arrival of SIGSEGV(11) {errno=0, code=1, addr=0x00000054}
 Partitioner: starting pass 2: 1148 functions, 29621 insns assigned to 5236 blocks (ave 6 insn/blk)
 Partitioner: starting pass 5: 1197 functions, 104383 insns assigned to 23030 blocks (ave 5 insn/blk)
 Partitioner: starting pass 31: 1462 functions, 118369 insns assigned to 28267 blocks (ave 4 insn/blk)
 4474:1 57.897 0x08094233[36578]:   stack frames:
 4474:1 57.897 0x08094233[36578]:     #0: bp=0xbfffd3c4 ip=0x08094233 in function __current_locale_name
 4474:1 57.897 0x08094233[36578]:     #1: bp=0xbfffd3e4 ip=0x0806af47 in function dcgettext
 4474:1 57.897 0x08094233[36578]:     #2: bp=0xbfffd428 ip=0x0805bfa7 in function strerror_r
 4474:1 57.897 0x08094233[36578]:     #3: bp=0xbfffd448 ip=0x0805beea in function strerror
 4474:1 57.897 0x08094233[36578]:     #4: bp=0xbfffd488 ip=0x080487dd in function cleanup
 4474:1 57.897 0x08094233[36578]:     #5: bp=0xbfffd4a8 ip=0x0804acc2 in function def_handler
 4474:1 57.897 0x08094233[36578]:     #6: bp=0xbfffdad8 ip=0x0804bf28 in memory region syscall_tst.221.09(LOAD#0)
 4474:1 57.897 0x08094233[36578]:     #7: bp=0xbfffdbf0 ip=0x0804ff91 in function __vsnprintf
 4474:1 57.897 0x08094233[36578]:     #8: bp=0xbfffdc08 ip=0x0804eff6 in function __snprintf
 4474:1 57.897 0x08094233[36578]:     #9: bp=0xbfffe048 ip=0x0804adea in function tst_tmpdir
 4474:1 57.897 0x08094233[36578]:     #10: bp=0xbfffe078 ip=0x08048571 in function setup
 4474:1 57.897 0x08094233[36578]:     #11: bp=0xbfffe0c8 ip=0x080482a0 in function main
 4474:1 57.897 0x08094233[36578]:     #12: bp=0xbfffe138 ip=0x0804b80f in memory region syscall_tst.221.09(LOAD#0)
 4474:1 57.897 0x08094233[36578]:     #13: bp=0x00000000 ip=0x08048181 in memory region syscall_tst.221.09(LOAD#0)
 @endverbatim
     */
    class SignalCallback: public Callback {
    public:
        /** The reason for invoking the callback. */
        enum Reason {
            ARRIVAL,                    /**< Signal has arrived at the thread.  It may be added to the thread's list of pending
                                         *   signals if the signal is not being ignored.  The signal will be delivered to the
                                         *   thread, to be handled by the thread's signal handler or default action, when the
                                         *   thread's signal mask allows it (and DELIVERY callbacks will be made at that time).
                                         *   An arbitrary interval may elapse between the time a signal arrives at a thread and
                                         *   the time the signal is delivered to the thread. */
            DELIVERY,                   /**< Signal is being delivered to the thread.  The signal has previously arrived at the
                                         *   thread and conditions are suitable for its delivery.  Delivering a signal either
                                         *   causes some default action to occur (e.g., exit with a core dump), or a specimen's
                                         *   signal handler to run. */
        };

        /** Arguments passed to signal callbacks. */
        struct Args {
            Args(RSIM_Thread *thread, int signo, RSIM_SignalHandling::SigInfo *info, Reason reason)
                : thread(thread), signo(signo), info(info), reason(reason) {}
            /** The thread at which the signal has arrived or to which the signal is being delivered.  This might not be the
             *    same as the calling thread. */            
            RSIM_Thread *thread;
            
            /** The signal number.  Signal number zero is special: it is a valid number but refers to no signal.  Callbacks
             *  will not normally be invoked for signal zero. */
            int signo;

            /** Information about the signal being delivered.  Callbacks are permitted to modify this struct in order to
             *  influence the specimen. */
            RSIM_SignalHandling::SigInfo *info;

            /** Reason for invoking the callback. */
            Reason reason;
        };
        virtual bool operator()(bool prev, const Args&) = 0;
    };

    /** Registers a signal callback. These callbacks are invoked before and after (depending on the value of @p when) a signal
     *  arrives or is delivered.  The specified callback object is inserted into the list without copying it. See
     *  call_signal_callbacks() for details about how these callbacks are invoked.
     *
     *  Thread safety:  This method is thread safe. */
    void add_signal_callback(When, SignalCallback*);

    /** Unregisters a signal callback.  The most recently registered instance of the specified callback (if any) is removed
     *  from the pre- or post-signal callback list, depending on the value of @p when.  The removed callback object is not
     *  destroyed.  Returns true if a callback was removed, false if not.
     *
     *  Thread safety:  This method is thread safe. */
    bool remove_signal_callback(When, SignalCallback*);


    /** Removes all signal callbacks.  The pre- or post-signal callbacks are removed, depending on the value of @p when. None
     *  of the removed callbacks are destroyed.
     *
     *  Thread safety:  This method is thread safe. */
    void clear_signal_callbacks(When);

    /** Invokes all the signal callbacks.  The pre- or post-signal callbacks (depending on the value of @p when) are invoked in
     *  the order they were registered.  The specified @p prev value is passed to the first callback as its @p prev argument;
     *  subsequent callbacks' @p prev argument is the return value of the previous callback; the return value of the final
     *  callback becomes the return value of this method.  However, if no callbacks are invoked (because the list is empty)
     *  then this method's return value is the specified @p prev value.  The @p thread, signal number, and signal information
     *  struct is passed to each of the callbacks.
     *
     *  The simulator invokes this method before and after signals arrive and before and after they are delivered to the
     *  specified thread (not necessarily the calling thread).  The return value of the pre-signal callbacks determines whether
     *  the signal is actually arrives or is delivered (true) or is discarded (false).  That return value will be passed to the
     *  post-signal queue, which is invoked regardless of whether the signal arrived or was delivered.  The callbacks may
     *  modify the contents of the signal information structure to influence which signal arrives or is delivered.
     *
     *  The post-signal callbacks for signal delivery are invoked after the delivery decision is made but before the signal is
     *  handled by the specimen.  These callbacks are not invoked if the signal action is to exit the specimen.
     *
     *  Thread safety:  This method is thread safe.  The callbacks may register and/or unregister themselves or other callbacks
     *  from this RSIM_Callbacks object, but those actions do not affect which callbacks are made by this invocation of
     *  call_thread_callbacks(). */
    bool call_signal_callbacks(When, RSIM_Thread *thread, int signo, RSIM_SignalHandling::SigInfo*,
                               SignalCallback::Reason, bool prev);


        


    /**************************************************************************************************************************
     *                                  Thread Callbacks
     **************************************************************************************************************************/
public:

    /** Thread callbacks invoked on every thread. */
    class ThreadCallback: public Callback {
    public:
        /** Arguments passed to thread callbacks. */
        struct Args {
            Args(RSIM_Thread *thread)
                : thread(thread) {}
            RSIM_Thread *thread;                /**< The thread on behalf of which this callback is made. */
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
     *  from the pre- or post-thread callback list, depending on the value of @p when.  The removed callback object is not
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
     *                                  Process Callbacks
     **************************************************************************************************************************/
public:

    /** Callbacks invoked for processes.
     *
     *  These callbacks are invoked on behalf of the entire simulated process at certain points in a process lifetime.
     *
     *  Here's an example that shows how to produce an assembly dump of the process' executable memory whenever it's about to
     *  dump core rather than dumping core:
     *
     *  @code
     *  class DisassembleAtCoreDump: public RSIM_Callbacks::ProcessCallback {
     *  public:
     *      // Allow threads, processes, and simulators to all share a single instance
     *      // of this callback.
     *      virtual DisassembleAtCoreDump *clone() { return this; }
     *
     *      // The callback itself. It does nothing unless the reason for being called
     *      // is because the process is dumping core.
     *      virtual bool operator()(bool retval, const Args &args) {
     *          if (args.reason==COREDUMP) {
     *              SgAsmBlock *block = args.process->disassemble();
     *              AsmUnparser().unparse(std::cout, block);
     *              retval = false; // avoid a core dump
     *          }
     *          return retval;
     *      }
     *  };
     *
     *  // Register the callback with the simulator, which will copy it into the
     *  // process when the process is created.  Or we could have just registered
     *  // it with the process directly.
     *  DisassembleAtCoreDump dacd;
     *  simulator.get_callbacks().add_process_callback(RSIM_Callbacks::BEFORE, &dacd);
     *  
     *  @endcode
     */
    class ProcessCallback: public Callback {
    public:

        /** Reason for invoking process callbacks. */
        enum Reason {
            START,                              /**< Process is starting execution. */
            FORK,                               /**< New process created via clone system call.  The callback is invoked by the
                                                 *   thread performing the clone() system call before the system call is made,
                                                 *   and by the main thread of the new process almost immediately after the
                                                 *   system call returns. */
            COREDUMP,                           /**< Process is about to dump core. If call_process_callbacks() return false
                                                 *   then the core dump is avoided. */
            FINISH,                             /**< Process has finished execution. */
        };

        /** Arguments passed to process callbacks. */
        struct Args {
            Args(RSIM_Process *process, Reason reason)
                : process(process), reason(reason) {}
            RSIM_Process *process;              /**< The process on behalf of which this callback is made. */
            Reason reason;                      /**< Reason for invoking the callback. */
        };
        virtual bool operator()(bool prev, const Args&) = 0;
    };

    /** Registers a process callback.  Process callbacks are invoked before a new process is created or when a process exits
     *  (depending on @p when).  The specified callback object is inserted into the list without copying it. See
     *  call_process_callbacks() for details about how these callbacks are invoked.
     *
     *  Thread safety:  This method is thread safe. */
    void add_process_callback(When, ProcessCallback*);

    /** Unregisters a process callback.  The most recently registered instance of the specified callback (if any) is removed
     *  from the pre- or post-process callback list, depending on the value of @p when).  The removed callback object is not
     *  destroyed.  Returns true if a callback was removed, false if not.
     *
     *  Thread safety:  This method is thread safe. */
    bool remove_process_callback(When, ProcessCallback*);


    /** Removes all process callbacks.  The pre- or post-process callbacks are removed, depending on the value of @p when. None
     *  of the removed callbacks are destroyed.
     *
     *  Thread safety:  This method is thread safe. */
    void clear_process_callbacks(When);

    /** Invokes all the process callbacks.  The pre- or post-process callbacks (depending on the value of @p when) are invoked
     *  in the order they were registered.  The specified @p prev value is passed to the first callback as its @p prev
     *  argument; subsequent callbacks' @p prev argument is the return value of the previous callback; the return value of the
     *  final callback becomes the return value of this method.  However, if no callbacks are invoked (because the list is
     *  empty) then this method's return value is the specified @p prev value.  The @p process is passed to each of the
     *  callbacks.
     *
     *  Thread safety:  This method is thread safe.  The callbacks may register and/or unregister themselves or other callbacks
     *  from this RSIM_Callbacks object, but those actions do not affect which callbacks are made by this invocation of
     *  call_process_callbacks(). */
    bool call_process_callbacks(When, RSIM_Process*, RSIM_Callbacks::ProcessCallback::Reason, bool prev);



    /**************************************************************************************************************************
     *                                  Data members
     **************************************************************************************************************************/
private:
    /* See init() if you add more vectors */
    Rose::Callbacks::List<InsnCallback> insn_pre;
    Rose::Callbacks::List<InsnCallback> insn_post;

    Rose::Callbacks::List<MemoryCallback> memory_pre;
    Rose::Callbacks::List<MemoryCallback> memory_post;

    Rose::Callbacks::List<SyscallCallback> syscall_pre;
    Rose::Callbacks::List<SyscallCallback> syscall_post;

    Rose::Callbacks::List<SignalCallback> signal_pre;
    Rose::Callbacks::List<SignalCallback> signal_post;

    Rose::Callbacks::List<ThreadCallback> thread_pre;
    Rose::Callbacks::List<ThreadCallback> thread_post;

    Rose::Callbacks::List<ProcessCallback> process_pre;
    Rose::Callbacks::List<ProcessCallback> process_post;
};

#endif /* ROSE_RSIM_Callbacks_H */
