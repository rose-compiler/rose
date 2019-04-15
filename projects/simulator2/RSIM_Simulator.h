#ifndef ROSE_RSIM_Simulator_H  
#define ROSE_RSIM_Simulator_H

/* Order matters */
#include "RSIM_Common.h"
#include "RSIM_SignalHandling.h"
#include "RSIM_Callbacks.h"
#include "RSIM_Futex.h"
#include "RSIM_Process.h"
#include "RSIM_Thread.h"
#include "RSIM_Tools.h"

#include <BinaryLoader.h>

#include <signal.h>

/** @mainpage RSIM: A ROSE x86 Simulator
 *
 * @section RSIM_Intro Introduction
 *
 * RSIM is a simulator library for 32-bit, Intel x86 Linux programs and runs on 32- or 64-bit Linux.  The RSIM library is
 * demonstrated by the "x86sim" program.
 *
 * @section RSIM_Definitions Definitions
 *
 * The "specimen" is the program being executed inside the simulator.
 *
 * The "host OS" is the system for which ROSE was compiled and on which the simulator is running.
 *
 * The "guest OS" is the environment (system calls, signals, etc) provided by the simulator to the specimen.
 *
 * @section RSIM_Tool The Simulator Tool
 * 
 * The x86sim.C program is an example demonstrating the use of the RSIM library.  See the README in that directory for a
 * complete description of the command-line usage and capabilities.
 *
 * @section RSIM_Design Design
 *
 * RSIM uses ROSE's binary support to simulate a process. Namely,
 *
 * <ul>
 *   <li>The memory address space of the specimen is represented by an instance of ROSE's MemoryMap class.  Keeping the
 *       speciment's address space separated from the simulator's address space allows the specimen to use its entire address
 *       space however it chooses, and it prevents the specimen from being able to access the simulator's address space.
 *
 *       The main drawback of this design is that every specimen memory access must go through the MemoryMap, and the simulator
 *       must marshal data between the specimen's and the simulator's address spaces when performing a system call on behalf of
 *       the specimen.  This has an impact on performance.</li>
 *
 *   <li>The specimen's instructions are simulated rather than executed directly by the CPU.
 *
 *       Simulating each instruction has a number of advantages:
 *       <ol>
 *         <li>The specimen can be for a different architecture (guest) than that on which the simulator is running (host).</li>
 *         <li>The simulator can handle priviledged and normal instructions in a uniform way.</li>
 *         <li>It is easy to modify the simulator to do something special for certain instructions.</li>
 *         <li>It allows the simulator to keep the specimen in a separate address space that doesn't overlap with the simulator's
 *             own address space.</li>
 *         <li>It provides a way for ROSE developers to gain confidence that ROSE's instruction semantics are working
 *             properly. These same instruction semantics are used for a wide variety of analysis within the ROSE library and
 *             in other tools. A bug in the implementation would likely cause the simulation to fail.</li>
 *       </ol>
 *
 *       The main disadvantage of simulating each instruction is that it is much slower than executing instructions directly.</li>
 *
 *   <li>The ROSE Disassembler is used to build an intermediate representation of each instruction before the instruction can be
 *       simulated.  Disassembly at some level is necessary in order to simulate instructions, and the level of disassembly
 *       that the simulator uses is the same as that which ROSE uses for all other binary analyses. Besides being necessary for
 *       simulation, this also gives us confidence that the disassembler is functioning properly.</li>
 *
 *   <li>Dynamic linking in the specimen is resolved as a side effect of simulation.  The simulator loads the specimen's main
 *       executable into an address space (MemoryMap) like the OS would do, and then begins simulation.  If the main executable
 *       has an interpreter, then the interpreter is also loaded and simulation starts in the interpreter. In this way, the
 *       simulator is able to simulate dynamically linked executables&mdash;it simulates the linker itself (i.e., it simulates the
 *       ld-linux.so interpreter).</li>
 *
 *   <li>Specimen system calls can be intercepted by hooking into the "INT 0x80" or "SYSENTER" intruction.  In either case, the
 *       instruction is intercepted and the simulator processes the system call, either by invoking a real system call on the
 *       specimen's behalf, or by adjusting the specimen's state to emulate the system call.</li>
 * </ul>
 *
 * @section RSIM_Usage Using the Simulator
 *
 * The simulator's main class is RSIM_Simulator, but users will most often want to use one of the derived classes that has
 * operating-system specific details, such as the RSIM_Linux32 class.
 *
 * Many of the actions that occur during the simulation can be tied to user-defined callbacks invoked before and/or after the
 * action.  The callbacks are organized into collections of lists of functors as described by the RSIM_Callbacks class.  The
 * callbacks are able to augment the normal processing, replace the normal processing, or skip the normal processing
 * altogether depending on the situation.  For instance, a pre-instruction callback can check for unsupported instructions and
 * skip them, or a pre-syscall callback can count the total number of system calls executed per thread.  See the RSIM_Callbacks
 * class for details and examples.
 *
 * System calls made by the specimen are dispatched through a system call table which can be queried or adjusted by the methods
 * described for the RSIM_Simulator::SystemCall class. This allows users to augment or replace the code that implements various
 * system calls.
 *
 * The behavior of a simulator can be modified by attaching adapters.  For instance, if you need a simulator that prints
 * hexdumps of all data transfered over a TCP network connection, you would instantiate an RSIM_Adapter::TraceTcpIO adapter and
 * attach it to the simulator.  Documentation can be found in the RSIM_Adapter namespace.
 *
 * We provide a number of tools and callbacks that we expect will be useful to end users.  These are contained in the
 * RSIM_Tools namespace.
 *
 * If you run dynamically linked, 32-bit specimens on an amd64 host, you'll need to use "i386 -LRB3" to run the simulator.
 *
 * @section RSIM_Example Example
 *
 * This example shows how to use the RSIM classes.
 *
 * @code
 * // RSIM_Linux32 is derived from RSIM_Simulator and contains
 * // details about how a 32-bit Linux kernel operates.
 * RSIM_Linux32 sim;
 *
 * // Configure the simulator
 * Settings settings = ...; //or parse command line
 * sim.configure(settings);
 *
 * // Create the initial process object by loading a program
 * // and initializing the stack.  This also creates the main
 * // thread, but does not start executing it.
 * sim.loadSpecimen(argc-1, argv+1);
 *
 * // Getready to execute by making the specified simulator
 * // active.  This sets up signal handlers, etc.
 * sim.activate();
 *
 * // Allow executor threads to run and return when the
 * // simulated process terminates.  The return value is the
 * // termination status of the simulated program, like what
 * // would be returned by waitpid().
 * sim.main_loop();
 *
 * // Restore original signal handlers, etc.  This isn't really
 * // necessary in this simple example since we're about to
 * // exit anyway.
 * sim.deactivate();
 *
 * // Describe the termination status on stderr and then exit
 * // ourselves with that same status.
 * sim.describe_termination(stderr);
 * sim.terminate_self();
 * @endcode
 */

/** Base class for simulating programs.  Derived classes contain details about particular guest operating systems.
 *
 *  An RSIM_Simulator instance represents a single process that associated with a specified executable specimen.  The specimen
 *  process shares certain resources with the simulator process, such as process ID, signal mask, etc.  Therefore, only one
 *  RSIM_Simulator should be active at a time (see activate()).
 *
 *  The simulator points to a single simulated process (RSIM_Process), which in turn points to one or more simulated threads
 *  (RSIM_Thread). Each simulated thread runs in a real thread, and just as a simulated process shares certain state with the
 *  simulator process, the simulated threads share certain state with the real threads.
 *
 *  Example of a very simple simulator.
 *
 *  @code
 *  #include <rose.h>
 *  #include <RSIM_Linux32.h>
 *  int main(int argc, char *argv[], char *envp[]) {
 *      RSIM_Linux32 s;                          // RSIM_Linux32 is derived from RSIM_Simulator
 *      s.loadSpecimen(argc-1, argv+1);          // Create a simulated process and its initial thread
 *      s.activate();                            // Allow other real processes to signal this one
 *      s.main_loop();                           // Simulate until exit
 *      s.deactivate();                          // Restore original signal handlers
 *      return 0;
 *  }
 *  @endcode
 */
class RSIM_Simulator {
public:
    static Sawyer::Message::Facility mlog;

    /** Properties settable from the command-line. */
    struct Settings {
        std::string tracingFileName;
        std::vector<TracingFacility> tracing;
        std::vector<CoreStyle> coreStyles;
        std::string interpreterName;
        std::vector<std::string> vdsoPaths;
        std::vector<std::string> vsyscallPaths;
        std::string semaphoreName;
        bool showAuxv;
        std::string binaryTraceName;
        bool nativeLoad;
        Settings()
            : showAuxv(false), nativeLoad(false) {}
    };

private:
    std::string exeName_;                               // Specimen name as given on command-line, original argv[0]
    std::vector<std::string> exeArgs_;                  // Specimen argv, eventually with PATH-resolved argv[0]
    Settings settings_;                                 // Settings from the command-line
    std::vector<Rose::BinaryAnalysis::RegisterDescriptor> syscallArgRegs_; // Registers that store system call arguments
    Rose::BinaryAnalysis::RegisterDescriptor syscallNumReg_; // Register that stores the system call number
    Rose::BinaryAnalysis::RegisterDescriptor syscallRetReg_; // Register to hold the system call result
    
public:
    /** Default constructor. Construct a new simulator object, initializing its properties to sane values, but do not create an
     *  initial process. */
    RSIM_Simulator()
        : global_semaphore(NULL),
          tracingFlags_(0), core_flags(CORE_ELF), btrace_file(NULL), active(0), process(NULL), entry_va(0) {
        ctor();
    }

    /** Command-line switches for the simulator. */
    static Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings &settings /*in,out*/);

    /** Configure simulator properties from command-line switches.
     *
     *  The ENVP is used only for the --showauxv switch and may be a null pointer.
     *
     *  Thread safety: This method is thread safe provided it is not invoked on the same object concurrently. Note, however,
     *  that it may call functions registered with atexit(). */
    void configure(const Settings &settings,  char **envp=NULL);

    /** Obtain simulator settings. These are the settings that are typically changed by the command-line.
     *
     * @{ */
    const Settings& settings() const { return settings_; }
    Settings& settings() { return settings_; }
    /** @} */

    /** Update first argument with full path name.
     *
     *  Replaces the first executable argument, corresponding to argv[0], with an absolute name. For linux, this is done by
     *  consulting the $PATH environment variable. */
    virtual void updateExecutablePath() = 0;

    /** Construct the AST for the main executable without further linking or mapping. */
    virtual SgAsmInterpretation* parseMainExecutable(RSIM_Process*);

    /** Set the name of the global semaphore.
     *
     *  A semaphore is needed in order to synchronize certain operations between simulators whose specimens might be
     *  interacting with each other through SysV or POSIX inter-process communication (IPC) facilities.  These facilities
     *  assume that certain instructions are atomic, but simulation of those instructions is not. Therefore, the simulator
     *  needs to wrap those instructions inside a mutual exclusion construct: a named POSIX semaphore.
     *
     *  If the specimens communicating with each other have a parent/child relationship, then the simulator will set up a
     *  semaphore that's used among the various related simulators.  The semaphore will be named "/ROSE-Simulator-#####" where
     *  ##### is the process ID of the first simulator.  However, if the specimens are not related to one another, and started
     *  from unrelated simulators, then a different name should be specified, and the name should be the same for all the
     *  affected simulators.  BTW, POSIX named semaphores are usually created in the /dev/shm directory.  The simulator
     *  normally unlinks them when it's done, but it might be necessary to manually clean up semaphores from crashed
     *  simulators.
     *
     *  It is not possible to set the semaphore name once the semaphore has been created.  The set_semaphore_name() will return
     *  true on success, false if the name could not be set.  If do_unlink is true, then the semaphore name is immediately
     *  unlinked from the file system (you probably don't want to unlink it if it's going to be needed by another simulator).
     *  When the semaphore is created and no name is specified, the default name is used and immediately unlinked.
     *
     *  @{ */
    bool set_semaphore_name(const std::string &name, bool do_unlink=false);
    const std::string &get_semaphore_name() const;
    sem_t *get_semaphore(bool *unlinked=NULL);
    /** @} */

    /** Load program and create process object.
     *
     *  The argument vector, @p args, should contain the name of the executable and any arguments to pass to the executable.
     *  The executable file is located by consulting the $PATH environment variable, and is loaded into memory creating an
     *  RSIM_Process with an and an initial RSIM_Thread. The return value is zero for success, or a negative error number on
     *  failure.
     *
     *  The version that takes an existing process ID will initialize the simulator by attaching to the process, initializing
     *  the simulator, and then detaching.  One should note that the operating system state associated with the process is not
     *  copied (e.g., open file descriptors in the process are not open in the simulator, etc.)--only memory and registers are
     *  initialized in the simulator.
     *
     *  Thread safety: This method is thread safe provided it is not invoked on the same object concurrently.
     *
     * @{ */
    int loadSpecimen(const std::vector<std::string> &args, pid_t existingPid=-1);
    int loadSpecimen(pid_t existingPid);
    /** @} */

    /** Property: executable name.
     *
     *  This is the executable name as set by @ref loadSpecimen.
     *
     * @{ */
    const std::string& exeName() const { return exeName_; }
    void exeName(const std::string &s) { exeName_ = s; }
    /** @} */

    /** Property: specimen arguments.
     *
     *  These are the argv values for the specimen. The argv[0] might be rewritten based on the $PATH.
     *
     * @{ */
    const std::vector<std::string>& exeArgs() const { return exeArgs_; }
    std::vector<std::string>& exeArgs() { return exeArgs_; }
    /** @} */

    /** Activate a new simulator. Only one simulator should be active at any given time since activating a simulator causes the
     *  that simulator to trap all signals.  It is not necessary to activate a simulator if the simulator does not expect to
     *  receive signals from other processes.  Activation is recursive and should be matched by an equal number of
     *  deactivate() calls.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on the same object from multiple threads. */
    void activate();

    /** A simulator should be deactivated when it is no longer needed.  Activation is recursive and should be matched by an
     *  equal number of deactivate() calls.  The outer-most deactivation causes signal handlers to be restored to the state
     *  they were in when the simulator was first activated (other deactivations are no-ops).
     *
     *  Thread safety:  This method is thread safe; it can be invoked on the same object from multiple threads. */
    void deactivate();

    /** Returns true if this simulator is active.
     *
     *  Thread safety: This method is thread safe; it can be invoked on the same object from multiple threads. */
    bool is_active() const;

    /** Returns a pointer to the currently active simulator, if any.
     *
     *  Thread safety: This method is thread safe; it can be invoked on the same object from multiple threads. */
    static RSIM_Simulator *which_active();

    /** Returns a pointer to the main process object. */
    RSIM_Process *get_process() const {
        return process;
    }

    /** Enter the main simulation loop. This loop will return when the specimen terminates. The return value is the specimen
     *  termination status as it would be reported by waitpid().  Since the current process' main thread is the executor for
     *  the main thread of the specimen, a terminated specimen doesn't actually terminate the main thread&mdash;it just causes
     *  this method to return the would-be exit status. */
    int main_loop();

    /** Human-friendly termination string. */
    std::string describe_termination();

    /** Terminates the calling process with the same exit status or signal as the specimen. The @p status argument should be
     *  the status returned by waitpid(). */
    void terminate_self();

    /**************************************************************************************************************************
     *                                  Callbacks
     **************************************************************************************************************************/

    /** Obtains the callbacks object associated with the simulator.
     *
     *  @{ */
    RSIM_Callbacks &get_callbacks() {
        return callbacks;
    }
    const RSIM_Callbacks get_callbacks() const {
        return callbacks;
    }
    /** @} */

    /** Install a callback object.
     *
     *  This is just a convenient way of installing a callback object.  It appends it to the BEFORE slot (by default) of the
     *  appropriate queue.  If @p everwhere is true (not the default) then it also appends the callback to the appropriate
     *  callback list of the process and all existing threads.  Regardless of whether a callback is applied to process and
     *  threads, whenever a new process is created it gets a clone of all the simulator callbacks, and whenever a new thread is
     *  created it gets a clone of all its process callbacks.
     *
     *  @{ */  // ******* Similar functions in RSIM_Process and RSIM_Thread ******
    void install_callback(RSIM_Callbacks::InsnCallback *cb,
                          RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    void install_callback(RSIM_Callbacks::MemoryCallback *cb,
                          RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    void install_callback(RSIM_Callbacks::SyscallCallback *cb,
                          RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    void install_callback(RSIM_Callbacks::SignalCallback *cb,
                          RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    void install_callback(RSIM_Callbacks::ThreadCallback *cb,
                          RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    void install_callback(RSIM_Callbacks::ProcessCallback *cb,
                          RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    /** @} */

    /** Remove a callback object.
     *
     *  This is just a convenient way of removing callback objects.  It removes up to one instance of the callback from the
     *  simulator and, if @p everwhere is true (not the default) it recursively calls the removal methods for the process and
     *  all threads.  The comparison to find a callback object is by callback address.  If the callback has a @p clone() method
     *  that allocates a new callback object, then the callback specified as an argument probably won't be found in the process
     *  or threads.
     *
     * @{ */
    void remove_callback(RSIM_Callbacks::InsnCallback *cb,
                         RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    void remove_callback(RSIM_Callbacks::MemoryCallback *cb,
                         RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    void remove_callback(RSIM_Callbacks::SyscallCallback *cb,
                         RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    void remove_callback(RSIM_Callbacks::SignalCallback *cb,
                         RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    void remove_callback(RSIM_Callbacks::ThreadCallback *cb,
                         RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    void remove_callback(RSIM_Callbacks::ProcessCallback *cb,
                         RSIM_Callbacks::When when=RSIM_Callbacks::BEFORE, bool everywhere=false);
    /** @} */
    

    /**************************************************************************************************************************
     *                                  System calls
     **************************************************************************************************************************/
public:
    /** Implementations for system calls.
     *
     *  Whenever a specimen makes a system call, perhaps by loading a system call number into the EAX register and executing
     *  the INT 0x80 instruction, the simulator invokes all RSIM_Callbacks::SyscallCallback functors.  If those callbacks
     *  return false then the system call is not simulated.  Otherwise, the simulator consults a system call lookup table to
     *  find three lists of callbacks for that specific system call.  If the table has no entry for that system call then the
     *  simulator produces a core dump of the specimen and terminates simulation.
     *
     *  Each system call implementation consists of three lists of callbacks. The first list, named "enter", contains callbacks
     *  that print the syscall entrance message when system call tracing is enabled; the @p body list does the real work of the
     *  system call; and the @p leave list print the system call return value(s).  For each list, the callbacks are invoked in
     *  ROSE_Callbacks::FORWARD order. Any of the callbacks may throw an exception, in which case the subsequent callbacks on
     *  that list and following lists are not invoked.  The reason for having three lists of callbacks rather than one callback
     *  that does all three actions is to make it easier for users to augment an existing system call without having to
     *  reimplement the enter/leave boilerplate, or to modify the boilerplate without having to reimplement the system call.
     *
     *  The system call table, indexed by call number, contains SystemCall objects (allocated on the heap) which in turn
     *  contain the three lists of callbacks, named "enter", "body", and "leave".  These lists contain Callback (a virtual base
     *  class) functors conforming to the requirements documented for ROSE_Callbacks in the ROSE Library Reference Manual. End
     *  users may subclass Callback as necessary to implement a system call.  Operations on these lists are generally thread
     *  safe, and a list can be modified even while its callbacks are executing (modifying the list doesn't immediately affect
     *  which callbacks are invoked).
     *
     *  Functions to access the syscall table are:
     *  <ul>
     *    <li>RSIM_Simulator::syscall_is_implemented() -- predicate to test whether syscall is defined.</li>
     *    <li>RSIM_Simulator::syscall_implementation() -- returns the SystemCall struct containg the three callback lists.</li>
     *    <li>RSIM_Simulator::syscall_define() -- convenience method for defining function-based implementations.</li>
     *  </ul>
     *
     *  The RSIM_Simulator::syscall_define() deserves special mention.  It's often more convenient to implement or augment a
     *  system call using a function (or three, if you need to provide or modify enter/leave behavior). Unfortunately, ROSE's
     *  ROSE_Callbacks mechanism only supports functors having specific calling conventions.  Therefore, we've defined a
     *  SystemCall::Function class (derived from SystemCall::Callback) whose constructor takes a function pointer and whose
     *  callback invokes that function.  Examples of this usage can be found throughout the simulator (e.g., see
     *  RSIM_Linux32::ctor()).
     *
     *  @section SystemCall_Example1 Example: Augmenting an Existing System Call
     *
     *  This example augments the sys_read() and sys_write() system calls in order to count the total number of calls and bytes
     *  transferred.  We use functors (rather than functions) in order to neatly encapsulate the counters.
     *
     *  @code
     *  class IOCounter: public RSIM_Simulator::SystemCall::Callback {
     *  public:
     *      size_t ncalls, nbytes;
     *      IOCounter(): ncalls(0), nbytes(0) {}
     *
     *      // The callback. Signature is dictated by ROSE_Callbacks. Args is a struct defined
     *      // in the base class and contains a thread pointer and the syscall number.
     *      bool operator()(bool b, const Args &args) {
     *          ncalls++;
     *          int32_t n = args.thread->syscall_arg(-1); // system call return value
     *          if (n>0)
     *              nbytes += (size_t)n;
     *          return b; // per ROSE_Callbacks semantics
     *      }
     *  };
     *  @endcode
     *
     *  The next step is to instantiate one of these system call functors and register it for the appropriate system calls,
     *  namely sys_read (#3) and sys_write (#4). We don't worry about other I/O system calls for this example.  Our functor
     *  must be invoked after the read or write is simulated because we need to access the syscall's return value, so we append
     *  reather than prepend.
     *
     *  @code
     *  RSIM_Linux32 sim;
     *  IOCounter *ioc = new IOCounter;
     *  ...
     *  sim.syscall_implementation(3)->body.append(ioc);
     *  sim.syscall_implementation(4)->body.append(ioc);
     *  ...
     *  sim.main_loop();
     *  std::cout <<ioc->ncalls <<" I/O calls transfered " <<ioc->nbytes <<" bytes of data" <<std::endl;
     *  @endcode
     *
     *  @section Syscall_Example2 Example: Implementing a New System Call
     *
     *  This example shows how to implement a new system call&mdash;a delay function.  It's often useful for a specimen to
     *  pause for a specified number of seconds. Although the specimen could do that by calling the standard C sleep() function
     *  which ultimately invokes Linux's sys_sleep(), doing so also invokes additional system calls and generates a
     *  signal. These extra actions may interfere with other aspects that we're studying in the specimen.  Another way to pause
     *  would be to have the specimen execute a loop, but this results in a variable delay length depending on the hardware and
     *  system load, and it greatly increases the number of simulated instructions.  These actions also might interfere with
     *  some aspect we're studying, such as instruction tracing.  The best approach is to implement a new system call in which
     *  the simulator pauses on behalf of the specimen.  From the specimen's point of view, it makes a single system call which
     *  returns at a later time.
     *
     *  The first step is to choose an appropriate system call number which doesn't interfere with existing system calls. One
     *  way to do that is to guess a number and then query the system call table with RSIM_Simulator::syscall_is_implemented().
     *  Another way, and the one we use here, is to just pick a number arbitrarily: we use 600 because it's well above any
     *  numbers already used by the Linux kernel.
     *
     *  The second step is to implement three functions: one to print the system call name and arguments at the time the call
     *  is made, a second to implement the actual system call action, and a third to print the result when the call returns.
     *  Although its possible to place all three actions in a single function, we separate them out to make it easier for
     *  others to augment our implementation.  We choose to use functions rather than functors because this is a cleaner, more
     *  intuitive approach.
     *
     *  The function to print the system call name and arguments is shown below.  The second argument to syscall_enter() is a
     *  string specifying the types of arguments for the system call. In this example there's one signed integer argument: the
     *  number of seconds to delay.  The format specifiers are documented in the RSIM_Thread::syscall_enter() method.  The
     *  syscall_enter() method should be used rather than printing directly to a file because:
     *
     *  <ul>
     *    <li>It is automatically enabled or disabled according to the state of the system call tracing facility for the calling
     *        thread.  The state is usually inherited from the process, which inherits its state from the simulator, which
     *        is initialized according to the "--debug" command-line switch.</li>
     *    <li>It is less code, which once one is familiar with the format specifiers, is easier to read than equivalent code
     *        that prints each argument explicitly.</li>
     *    <li>It organizes output for human consumption when multiple threads are making system calls concurrently.  For
     *        instance, if another thread makes a system call after we're entered our system call but before we've returned,
     *        the output will clearly indicate such without causing the two system call outputs to be mixed together in some
     *        arbitrary, confusing manner.</li>
     *    <li>It automatically redirects output to log files if so configured.</li>
     *    <li>It will likely be more portable to call syscall_enter() rather than explicitly print each argument when we add
     *        support for abstract analysis.</li>
     *  </ul>
     *
     *  @code
     *  static void delay_enter(RSIM_Thread *t, int callno) {
     *      t->syscall_enter("delay", "d");
     *  }
     *  @endcode
     *
     *  Next we define the function that implements the system call. We grab the first system call argument using
     *  syscall_arg(), which is more portable than reading it directly from a machine register or the stack, then pause for the
     *  specified amount of time, and then set the system call's return value (non-negative typically indicates success).  Note
     *  that since the simulator is calling sleep() rather than the specimen, all the related system calls and signals happen
     *  directly within the simulator and don't affect the specimen.  This was our goal.  We're using sleep() for simplicity
     *  rather than nanosleep(), so the multi-threading issues that affect sleep() will also affect our system call.
     *
     *  @code
     *  static void delay_body(RSIM_Thread *t, int callno) {
     *      int nsec = t->syscall_arg(0); // the first argument
     *      while (nsec>0)
     *         nsec = sleep(nsec);
     *      t->syscall_return(0); // system call's return value; zero for success
     *  }
     *  @endcode
     *
     *  We define the final function to print the result. The syscall_leave() argument is a string that describes what should
     *  be printed.  As described in the documentation for that method, the letter "d" in the first position indicates that the
     *  return value is an integer where certain negative values indicate various error conditions.  In our case, we're always
     *  returning zero due to the syscall_return() call in the body function.  Using syscall_leave() has the same benefits as
     *  syscall_enter() above.
     *
     *  @code
     *  static void delay_leave(RSIM_Thread *t, int callno) {
     *      t->syscall_leave("d");
     *  }
     *  @endcode
     *
     *  The only thing remaining to do is to register our new system call. Failing to register a system call will, by default,
     *  result in a simulated core dump when the specimen invokes that system call.
     *
     *  @code
     *  int main() {
     *      RSIM_Linux32 simulator;
     *      static const int SYS_DELAY = 600;
     *      ...
     *      assert(!simulator.syscall_is_implemented(SYS_DELAY));
     *      simulator.syscall_define(SYS_DELAY, delay_enter, delay_body, delay_leave);
     *      ...
     *  }
     *  @endcode
     *
     *  Within the specimen, our system call must be invoked via the syscall() function because the C library doesn't know
     *  about it:
     *
     *  @code
     *  #include <stdio.h>
     *  #include <syscall.h>
     *
     *  void delay(int n) {
     *      int result = syscall(600, n);
     *      if (-1==result && ENOSYS==errno)
     *          printf("we're not running in the simulator\n");
     *  }
     *  @endcode
     * 
     *  @section Syscall_Example3 Example: One-shot Augmentation
     *
     *  Here's one way to print a special message the first time a system call is invoked.  For simplicity, we haven't worried
     *  about multiple threads&mdash;it's possible for two threads to invoke the same system call concurrently, in which case
     *  both might print the message before either removes the functor from the callback list (and recall that removal of a
     *  functor does not take effect until the next time the list is traversed).
     *
     *  @code
     *  class NotifyOnce: public RSIM_Simulator::SystemCall::Callback {
     *  public:
     *      bool operator()(bool b, const Args &args) {
     *          args.thread->tracing(TRACE_SYSCALL) <<"[FIRST CALL]";
     *          RSIM_Simulator *sim = args.thread->get_process()->get_simulator();
     *          sim->syscall_implementation(args.callno)->enter.erase(this);
     *          return b;
     *      }
     *  };
     *  @endcode
     *
     *  A single functor can be used to augment the "enter" action for multiple system calls (or we can use multiple
     *  instances).
     *
     *  @code
     *  RSIM_Linux32 *simulator = ...;
     *  NotifyOnce notifier;
     *  simulator->syscall_implementation(3)->enter.append(&notifier);
     *  simulator->syscall_implementation(4)->enter.append(&notifier);
     *  @endcode
     *
     *  @section Syscall_Example4 Example: Undefining a System Call
     *
     *  To completely remove a system call implementation and cause the simulator to dump the specimen's core if it tries to
     *  invoke that system call, simply remove all enter, body, and leave callbacks for that syscall.  Here we make sys_fork
     *  (#2) and sys_clone (#120) undefined:
     *
     *  @code
     *  RSIM_Linux32 *simulator = ...;
     *
     *  RSIM_Simulator::SystemCall *sc_fork  = simulator->syscall_implementation(2);
     *  sc_fork->enter.clear();
     *  sc_fork->body.clear();
     *  sc_fork->leave.clear();
     *
     *  RSIM_Simulator::SystemCall *sc_clone = simulator->syscall_implementation(120);
     *  sc_clone->enter.clear();
     *  sc_clone->body.clear();
     *  sc_clone->leave.clear();
     *  @endcode
     *
     *  @section Syscall_Example5 Example: Replacing a System Call
     * 
     *  Sometimes it's useful to make a system call not do anything except return an error. Here we make sys_chown (#182)
     *  return the "not implemented" error.
     *
     *  @code
     *  class NoOp: public RSIM_Simulator::SystemCall::Callback {
     *  public:
     *      bool operator()(bool b, const Args &args) {
     *          args.thread->tracing(TRACE_SYSCALL) <<"[NOOP]";
     *          args.thread->syscall_return(-ENOSYS);
     *          return b;
     *      }
     *  } syscall_noop;
     *
     *  RSIM_Simulator *simulator = ...;
     *  simulator->syscall_implementation(182)->body.clear().append(&syscall_noop);
     *  @endcode
     */
    struct SystemCall {

        /** Base class for system call implementations.  System calls are implemented using user-defined callbacks as described
         *  in the documentation for ROSE_Callbacks. */
        struct Callback {
            struct Args {                                       /**< Arguments to pass to system call functors. */
                Args(RSIM_Thread *thread, int callno)
                    : thread(thread), callno(callno) {}
                RSIM_Thread *thread;                            /**< Thread making the system call. */
                int callno;                                     /**< System call identifying number (e.g., from unistd_32.h). */
            };
            virtual ~Callback() {}
            virtual bool operator()(bool b, const Args&) = 0;
        };

        /** System call implementation based on a function.  This kind of callback is used by RSIM_Simulator::syscall_define()
         *  when the system call has been implemented as a function rather than a class. */
        struct Function: public Callback {
            Function(void(*func)(RSIM_Thread*, int callno))
                : func(func) {}
            void (*func)(RSIM_Thread*, int callno);
            bool operator()(bool b, const Args &a) {
                if (b)
                    func(a.thread, a.callno);
                return b;
            }
        };

        ROSE_Callbacks::List<Callback> enter;                   /**< Callbacks for entering a system call. */
        ROSE_Callbacks::List<Callback> body;                    /**< Callbacks for system call body (the real work). */
        ROSE_Callbacks::List<Callback> leave;                   /**< Callbacks for returning from a system call. */
    };

    /** Returns true if the specified system call exists in the system call table and has at least one callback.
     *
     *  Thread safety:  This method is thread safe. */
    bool syscall_is_implemented(int callno) const;

    /** Returns a reference to the struct containing callback lists for a specified system call.  If the system call number is
     *  not defined yet, then we get back a new SystemCall struct with empty callback lists.
     *
     *  Thread safety:  This method is thread safe.  Once a system call is created, the SystemCall structure is allocated and
     *  then never deleted.  The system call can be effectively de-implemented by clearing all the callback lists for that
     *  system call. */
    SystemCall* syscall_implementation(int callno);

    /** Defines a system call implemented as function(s).  The system call is implemented as three functions (any combination
     *  of which may be null pointers).  The @p enter function uses the tracing facility to produce messages about entering the
     *  system call; the @p body function implements the real work of the system call; and the @p leave function uses the
     *  tracing facility to produce messages about the return from the system call.  All functions are appended to the end of
     *  the callback lists for the system call.
     *
     *  This function is intended as a convenience.  More control of registering the implementation can be had by using the
     *  SystemCall structure directly via syscall_implementation() and either the SystemCall::Function class or a user-defined
     *  callback class.
     *
     *  Thread safety:  This method is thread safe. */
    void syscall_define(int callno,
                        void (*enter)(RSIM_Thread*, int callno),
                        void (*body )(RSIM_Thread*, int callno),
                        void (*leave)(RSIM_Thread*, int callno));

    /** Property: Register that stores a syscall return value.
     *
     * @{ */
    Rose::BinaryAnalysis::RegisterDescriptor syscallReturnRegister() const { return syscallRetReg_; }
    void syscallReturnRegister(Rose::BinaryAnalysis::RegisterDescriptor r) { syscallRetReg_ = r; }
    /** @} */

    /** Property: System call argument registers.
     *
     * @{ */
    const std::vector<Rose::BinaryAnalysis::RegisterDescriptor>& syscallArgumentRegisters() const { return syscallArgRegs_; }
    std::vector<Rose::BinaryAnalysis::RegisterDescriptor>& syscallArgumentRegisters() { return syscallArgRegs_; }
    /** @} */

private:
    std::map<int/*callno*/, SystemCall*> syscall_table;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Architecture-specific stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns true if this simulator supports the speciment type. */
    virtual bool isSupportedArch(SgAsmGenericHeader*) = 0;

    /** Initialize registers. This happens once while the process is being loaded and before its stack is initialized. The
     * instruction pointer need not be initialized here since the loader will do that later. */
    virtual PtRegs initialRegistersArch(RSIM_Process*) = 0;

    /** Architecture specific loading and initialization using native method. */
    virtual void loadSpecimenNative(RSIM_Process*, Rose::BinaryAnalysis::Disassembler*, int existingPid=-1) = 0;

    /** Architecture specific loading. */
    virtual void loadSpecimenArch(RSIM_Process*, SgAsmInterpretation*, const std::string &interpName) = 0;

    /** Initialize stack for main thread. */
    virtual void initializeStackArch(RSIM_Thread*, SgAsmGenericHeader *) = 0;

    /** Initialize the simulated operating system. */
    virtual void initializeSimulatedOs(RSIM_Process*, SgAsmGenericHeader*) = 0;

    /** Called immediately after a new thread is created. */
    virtual void threadCreated(RSIM_Thread*) {}

    /***************************************************************************************************************************/
    
private:
    /** Helper for object construction. */
    void ctor();

    /** Create the main process object for the simulator. The process is initialized from properties already initialized in the
     *  simulator.  This method is normally called from constructors.  A single simulator simulates a single process that
     *  shares certain state (such as PID) with the real process running the simulator. */
    RSIM_Process *create_process();

    /** Signal handler called when some other process sends the simulated process a signal.  This handler simply places the
     *  signal on the process's signal queue.  Ths RSIM_Simulator::activate() registers this signal handler. */
    static void signal_receiver(int signo, siginfo_t*, void*);

    /** Signal handler that does nothing, but whose side effect is to interrupt blocked system calls.  This handler is
     * registered by RSIM_Simulator::activate(). */
    static void signal_wakeup(int signo);

    std::string global_semaphore_name;  /**< Name of global simulator semaphore. If empty, a name is created when needed. */
    bool global_semaphore_unlink;       /**< If set, then immediately unlink the semaphore from the file system. */
    sem_t *global_semaphore;            /**< The global simulator semaphore. */

private:
    /* Configuration variables */
    unsigned tracingFlags_;             /**< What things should be traced for debugging? (See TraceFlags enum) */
    unsigned core_flags;                /**< Kinds of core dumps to produce. (See CoreStyle enum) */

private:
    FILE *btrace_file;                  /**< Name for binary trace file, which will log info about process execution. */

    /* Simulator activation/deactivation */
    unsigned active;                    /**< Levels of activation. See activate(). */
    static RSIM_Simulator *active_sim;  /**< Points to the currently active simulator (there can be at most one). */
    int signal_installed[_NSIG];        /**< (N>0 implies installed; N==0 implies not; N<0 implies error where N==-errno). */
    struct sigaction signal_restore[_NSIG];/**< When active and installed, this is the signal action to restore to deactivate. */

    /* Other */
    RSIM_Process *process;              /**< Main process. */
    rose_addr_t entry_va;               /**< Main entry address. */
    static SAWYER_THREAD_TRAITS::RecursiveMutex class_rwlock;   /**< For methods that access class variables. */
    mutable SAWYER_THREAD_TRAITS::RecursiveMutex instance_rwlock;/**< Read-write lock per simulator object. */
    RSIM_Callbacks callbacks;           /**< Callbacks used to initialize the RSIM_Process callbacks. */
};



#endif /* ROSE_RSIM_Simulator_H */
