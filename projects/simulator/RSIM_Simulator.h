#ifndef ROSE_RSIM_Simulator_H
#define ROSE_RSIM_Simulator_H

/* Being the top-level header file for ROSE's simulator support, this header should include everything one would normally need
 * for the basic simulator.  However, additional architecture- and OS-specific headers may be needed to actually instantiate a
 * simulator. */
#include "threadSupport.h"

#include "RSIM_Common.h"
#include "RSIM_Callbacks.h"
#include "RSIM_SignalHandling.h"
#include "RSIM_Process.h"
#include "RSIM_SemanticPolicy.h"
#include "RSIM_Thread.h"
#include "RSIM_Templates.h"

#include <signal.h>

/** @mainpage RSIM: A ROSE x86 Simulator
 *
 * @section Intro Introduction
 *
 * RSIM is a simulator library for 32-bit, Intel x86 Linux programs and runs on 32- or 64-bit Linux.  The RSIM library is
 * demonstrated by the "x86sim" program.
 *
 * @section Definitions Definitions
 *
 * The "specimen" is the program being executed inside the simulator.
 *
 * The "host OS" is the system for which ROSE was compiled and on which the simulator is running.
 *
 * The "guest OS" is the environment (system calls, signals, etc) provided by the simulator to the specimen.
 *
 * @section Usage Usage
 * 
 * The x86sim.C program is an example demonstrating the use of the RSIM library.  See the README in that directory for a
 * complete description of the command-line usage and capabilities.
 *
 * @section Design Design
 *
 * RSIM uses ROSE's binary support to simulate a process. Namely,
 *
 * <ul>
 *   <li>The memory address space of the specimen is represented by an instance of ROSE's MemoryMap class.  Keeping the
 *       speciment's address space separated from the simulator's address space allows the specimen to use its entire address
 *       space however it chooses, and it prevents the specimen from being able to access the simulator's address space.
 *
 *       The main drawback of this design is that every specimen memory access must go through the MemoryMap, and the simulator
 *       must marchal data between the specimen's and the simulator's address spaces when performing a system call on behalf of
 *       the specimen.  This has an impact on performance.</li>
 *
 *   <li>The specimen's instructions are simulated rather than executed directly by the CPU.  The simulation is performed by
 *       the X86InstructionSemantics class defined in ROSE and an RSIM_SemanticPolicy class defined in the simulator.  The
 *       X86InstructionSemantics class defines what basic operations must be performed by each instruction, while the
 *       RSIM_SemanticPolicy class defines the operations themselves.
 *
 *       Simulating each instruction has a number of advantages:
 *       <ol>
 *         <li>The specimen can be for a different architecture (guest) than that on which the simulator is running (host).</li>
 *         <li>The simulator can handle priviledged and normal instructions in a uniform way.</li>
 *         <li>It is easy to modify the simulator to do something special for certain instructions.</li>
 *         <li>It allows the simulator to keep the specimen in a separate address space that doesn't overlap with the simulator's
 *             own address space.</li>
 *         <li>It provides a way for ROSE developers to gain confidence that ROSE's instruction semantics are working properly.
 *             A bug in the implementation would likely cause the specimen to fail.</li>
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
 *       simulator is able to simulate dynamically linked executables -- it simulates the linker itself (i.e., it simulates the
 *       ld-linux.so interpreter).</li>
 *
 *   <li>Specimen system calls can be intercepted by hooking into the "INT 0x80" or "SYSENTER" intruction.  In either case, the
 *       instruction is intercepted and the simulator processes the system call, either by invoking a real system call on the
 *       specimen's behalf, or by adjusting the specimen's state to emulate the system call.</li>
 * </ul>
 *
 * @section Example Example
 *
 * This example shows how to use the RSIM classes.
 *
 * @code
 * RSIM_Simulator sim;
 *
 * // Configure the simulator from command-line switches.  The
 * // return value is the index of the executable name in argv.
 * int n = sim.configure(argc, argv, envp);
 *
 * // Create the initial process object by loading a program
 * // and initializing the stack.  This also creates the main
 * // thread, but does not start executing it.
 * sim.exec(argc-n, argv+n);
 *
 * // Get ready to execute by making the specified simulator
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
 *  Example of a very simple simulator.  The simulator is configured from the command-line and the name and arguments of the
 *  executable specimen are supplied:
 *
 *  @code
 *  #include <rose.h>
 *  #include <RSIM_Linux32.h>
 *  int main(int argc, char *argv[], char *envp[]) {
 *      RSIM_Linux32 s;                          // RSIM_Linux32 is derived from RSIM_Simulator
 *      int n = s.configure(argc, argv, envp);   // Configure the simulator
 *      s.exec(argc-n, argv+n);                  // Create a simulated process and its initial thread
 *      s.activate();                            // Allow other real processes to signal this one
 *      s.main_loop();                           // Simulate until exit
 *      s.deactivate();                          // Restore original signal handlers
 *      return 0;
 *  }
 *  @endcode
 */
class RSIM_Simulator {
public:
    /** Default constructor. Construct a new simulator object, initializing its properties to sane values, but do not create an
     *  initial process. */
    RSIM_Simulator()
        : tracing_flags(0), core_flags(CORE_ELF), btrace_file(NULL), active(0), process(NULL), entry_va(0) {
        ctor();
    }

    /** Configure simulator properties from command-line switches.  The ENVP is used only for the --showauxv switch and may be
     *  a null pointer.  This should normally be called only by constructors, before the main process is created.  The return
     *  value is the number of switch arguments that were processed.  In other words, argv[parse_cmdline(...)] is probably the
     *  name of the executable to load and simulate, and the subsequent arguments are the arguments to pass to that executable.
     *
     *  Thread safety: This method is thread safe provided it is not invoked on the same object concurrently. Note, however,
     *  that it may call functions registered with atexit(). */
    int configure(int argc, char **argv,  char **envp=NULL);

    /** Load program and create process object.  The argument vector, @p argv, should contain the name of the executable and
     *  any arguments to pass to the executable.  The executable file is located by consulting the $PATH environment variable,
     *  and is loaded into memory and an initial RSIM_Thread is created.  The calling thread is the executor for the
     *  RSIM_Thread.  The return value is zero for success, or a negative error number on failure. This should not be confused
     *  with the execve system call.
     *
     *  Thread safety: This method is thread safe provided it is not invoked on the same object concurrently. */
    int exec(int argc, char **argv);

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
     *  the main thread of the specimen, a terminated specimen doesn't actually terminate the main thread -- it just causes
     *  this method to return the would-be exit status. */
    int main_loop();

    /** Prints termination status in a human friendly manner. */
    void describe_termination(FILE*);

    /** Terminates the calling process with the same exit status or signal as the specimen. The @p status argument should be
     *  the status returned by waitpid(). */
    void terminate_self();

    /**************************************************************************************************************************
     *                                  Callbacks
     **************************************************************************************************************************/

    //@{
    /** Obtains the callbacks object associated with the simulator. */
    RSIM_Callbacks &get_callbacks() {
        return callbacks;
    }
    const RSIM_Callbacks get_callbacks() const {
        return callbacks;
    }
    //@}

    /**************************************************************************************************************************
     *                                  System calls
     **************************************************************************************************************************/
public:
    /** Callbacks that implement a particular system call.  The @p enter callback is responsible for printing the syscall
     *  entrance message when system call tracing is enabled; the @p body does the real work of the system call; and the @p
     *  leave callback is to print the system call return value(s).  Any of the callbacks may throw an exception, in which case
     *  the subsequent callbacks are not invoked. */
    struct SystemCall {
        SystemCall()
            :enter(NULL), body(NULL), leave(NULL) {}
        SystemCall(void(*enter)(RSIM_Thread*, int), void(*body)(RSIM_Thread*, int), void(*leave)(RSIM_Thread*, int))
            :enter(enter), body(body), leave(leave) {}
        void (*enter)(RSIM_Thread*, int callno);
        void (*body)(RSIM_Thread*, int callno);
        void (*leave)(RSIM_Thread*, int callno);
    };

    /** Modifies system call emulation.  Sets the callbacks associated with the specified system call.
     *
     *  Thread safety:  This method is thread safe. */
    void syscall_set(int callno, const SystemCall&);

    /** Returns the functions implementing the specified system call.
     *
     *  Thread safety:  This method is thread safe. */
    SystemCall syscall_get(int callno) const;

private:
    std::map<int, SystemCall> syscall_table;


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


private:
    /* Configuration variables */
    std::string tracing_file_name;      /**< Name pattern for debug trace output, or empty to disable. */
    unsigned tracing_flags;             /**< What things should be traced for debugging? (See TraceFlags enum) */
    unsigned core_flags;                /**< Kinds of core dumps to produce. (See CoreStyle enum) */
    std::string interp_name;            /**< Name of command-line specified interpreter for dynamic linking. */
    std::vector<std::string> vdso_paths;/**< Files and/or directories to search for a virtual dynamic shared library. */
    FILE *btrace_file;                  /**< Name for binary trace file, which will log info about process execution. */

    /* Simulator activation/deactivation */
    unsigned active;                    /**< Levels of activation. See activate(). */
    static RSIM_Simulator *active_sim;  /**< Points to the currently active simulator (there can be at most one). */
    int signal_installed[_NSIG];        /**< (N>0 implies installed; N==0 implies not; N<0 implies error where N==-errno). */
    struct sigaction signal_restore[_NSIG];/**< When active and installed, this is the signal action to restore to deactivate. */

    /* Other */
    RSIM_Process *process;              /**< Main process. */
    rose_addr_t entry_va;               /**< Main entry address. */
    static RTS_rwlock_t class_rwlock;   /**< For methods that access class variables. */
    mutable RTS_rwlock_t instance_rwlock;/**< Read-write lock per simulator object. */
    RSIM_Callbacks callbacks;           /**< Callbacks used to initialize the RSIM_Process callbacks. */
};



#endif /* ROSE_RSIM_Simulator_H */
