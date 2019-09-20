#ifndef ROSE_BinaryAnalysis_Debugger_H
#define ROSE_BinaryAnalysis_Debugger_H

#include <BitFlags.h>
#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <Disassembler.h>
#include <Sawyer/BitVector.h>

namespace Rose {
namespace BinaryAnalysis {

/** Shared-ownership pointer to @ref Debugger. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<class Debugger> DebuggerPtr;

/** Simple debugger.
 *
 *  This class implements a very simple debugger. */
class Debugger: private boost::noncopyable, public Sawyer::SharedObject {
public:
    /** Shared-ownership pointer to @ref Debugger. See @ref heap_object_shared_ownership. */
    typedef Sawyer::SharedPointer<Debugger> Ptr;

    /** How to detach from a process when this object is destroyed. */
    enum DetachMode {
        KILL,                                           /**< Kill the process. */
        DETACH,                                         /**< Simply detach leaving process in current state. */
        CONTINUE,                                       /**< Detach from process and cause it to continue running. */
        NOTHING                                         /**< Do nothing. */
    };

    /** Flags controlling operation. */
    enum Flag {
        ATTACH          = 0x00000001,                   /**< Attach to existing process. */
        REDIRECT_INPUT  = 0x00000002,                   /**< Redirect input from /dev/null. */
        REDIRECT_OUTPUT = 0x00000004,                   /**< Redirect output to /dev/null. */
        REDIRECT_ERROR  = 0x00000008,                   /**< Redirect standard error to /dev/null. */
        CLOSE_FILES     = 0x00000010,                   /**< Close all file descriptors > 2. */
        DEFAULT_FLAGS   = 0x00000013                    /**< Default flags. */
    };

    /** Describes the specimen to be debugged.
     *
     *  A specimen can be either an executable program or a running process. */
    class Specimen {
        BitFlags<Flag> flags_;                          // operational flags

        // Members for running a program
        boost::filesystem::path program_;               // ;full path of executable program
        std::vector<std::string> arguments_;            // command-line arguments of executable program
        boost::filesystem::path workingDirectory_;      // name or working directory, or use CWD if empty

        // Members for attaching to a process
        int pid_;                                       // process ID (int instead of pid_t for portability)

    public:
        /** Default construct an empty specimen descriptor. */
        Specimen()
            : pid_(-1) {}
        
        /** Construct a specimen description for a process. */
        Specimen(int pid) /*implicit*/
            : flags_(DEFAULT_FLAGS), pid_(pid) {}

        /** Construct a specimen description for a program with no arguments. */
        Specimen(const boost::filesystem::path &name) /*implicit*/
            : flags_(DEFAULT_FLAGS), program_(name), pid_(-1) {}

        /** Construct a specimen description for a program with arguments. */
        Specimen(const boost::filesystem::path &name, const std::vector<std::string> &args)
            : flags_(DEFAULT_FLAGS), program_(name), arguments_(args), pid_(-1) {}

        /** Construct a specimen description from combined program and arguments. */
        Specimen(const std::vector<std::string> &nameAndArgs) /*implicit*/
            : flags_(DEFAULT_FLAGS), program_(nameAndArgs.front()),
              arguments_(nameAndArgs.begin()+1, nameAndArgs.end()), pid_(-1) {
        }
        
    public:
        /** Property: Name of executable program to run.
         *
         *  A specimen description can be either a program or a process, but not both. Setting this property will
         *  clear the @ref process property.
         *
         * @{ */
        boost::filesystem::path program() const {
            return program_;
        }
        void program(const boost::filesystem::path &name) {
            program_ = name;
            pid_ = -1;
        }
        /** @} */

        /** Property: Program command-line arguments.
         *
         *  This property is the list of command-line arguments supplied to a program when starting it. This property is
         *  not used when attaching to an existing process.
         *
         * @{ */
        const std::vector<std::string>& arguments() const {
            return arguments_;
        }
        void arguments(const std::vector<std::string> &args) {
            arguments_ = args;
        }
        /** @} */

        /** Property: Current working directory for running a program.
         *
         *  This property is only used for starting a new program, not for attaching to a process (which already has
         *  a current working directory). If no working directory is specified then the program's working directory will
         *  be this process's current working directory.
         *
         * @{ */
        boost::filesystem::path workingDirectory() const {
            return workingDirectory_;
        }
        void workingDirectory(const boost::filesystem::path &name) {
            workingDirectory_ = name;
        }
        /** @} */

        /** Property: Operational flags.
         *
         *  These flags control some finer aspects of the operations.
         *
         * @{ */
        const BitFlags<Flag>& flags() const {
            return flags_;
        }
        BitFlags<Flag>& flags() {
            return flags_;
        }
        /** @} */

        /** Property: Process ID.
         *
         *  This is the identification number for a specimen process to which the debugger should be attached. Setting
         *  this property will clear the @ref program property.
         *
         * @{ */
        int process() const {
            return pid_;
        }
        void process(int pid) {
            pid_ = pid;
            program_.clear();
        }
        /** @} */

        /** Print some basic info about the specimen. */
        void print(std::ostream &out) const;
    };

private:
    typedef Sawyer::Container::Map<RegisterDescriptor, size_t> UserRegDefs;
    enum RegPageStatus { REGPAGE_NONE, REGPAGE_REGS, REGPAGE_FPREGS };

    Specimen specimen_;                                 // description of specimen being debugged
    int child_;                                         // process being debugged (int, not pid_t, for Windows portability)
    DetachMode howDetach_;                              // how to detach from the subordinate
    int wstat_;                                         // last status from waitpid
    AddressIntervalSet breakpoints_;                    // list of breakpoint addresses
    int sendSignal_;                                    // pending signal
    UserRegDefs userRegDefs_;                           // how registers map to user_regs_struct in <sys/user.h>
    UserRegDefs userFpRegDefs_;                         // how registers map to user_fpregs_struct in <sys/user.h>
    size_t kernelWordSize_;                             // cached width in bits of kernel's words
    uint8_t regsPage_[512];                             // latest register information read from subordinate
    RegPageStatus regsPageStatus_;                      // what are the contents of regPage_?
    Disassembler *disassembler_;                        // how to disassemble instructions

    //----------------------------------------
    // Real constructors
    //----------------------------------------
protected:
    Debugger()
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE),
          disassembler_(NULL) {
        init();
    }

    /** Construct a debugger attached to a specimen. */
    explicit Debugger(const Specimen &specimen)
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE),
          disassembler_(NULL) {
        init();
        attach(specimen);
    }

public:
    ~Debugger() {
        detach();
    }

    //----------------------------------------
    // Static allocating constructors
    //----------------------------------------
public:
    /** Create a debugger object that isn't attached to any subordinate process. */
    static Ptr instance() {
        return Ptr(new Debugger);
    }

    /** Create a debugger and start debugging a specimen. */
    static Ptr instance(const Specimen &specimen) {
        return Ptr(new Debugger(specimen));
    }
    
    //----------------------------------------
    // Attaching to subordinate
    //----------------------------------------
public:
    /** Attach to a specimen. */
    void attach(const Specimen&);

    /** Returns true if attached to a subordinate.  Return value is the subordinate process ID. */
    int isAttached() { return child_; }

    /** Detach from the subordinate. */
    void detach();

    /** Terminate the subordinate. */
    void terminate();

    //----------------------------------------
    // Operations on a subordinate
    //----------------------------------------
public:
    /** Set execution address. */
    void executionAddress(rose_addr_t va);

    /** Get execution address. */
    rose_addr_t executionAddress();

    /** Set breakpoints. */
    void setBreakpoint(const AddressInterval&);

    /** Remove breakpoints. */
    void clearBreakpoint(const AddressInterval&);

    /** Remove all breakpoints. */
    void clearBreakpoints() { breakpoints_.clear(); }

    /** Execute one instruction. */
    void singleStep();

    /** Run until the next breakpoint is reached. */
    void runToBreakpoint();

    /** Run until the next system call.
     *
     *  The subordinate is run until it is about to make a system call or has just returned from a system call, or it has
     *  encountered a signal or terminated.  Execution does not stop at break points. */
    void runToSyscall();

    /** Obtain and cache kernel's word size in bits.  The wordsize of the kernel is not necessarily the same as the word size
     * of the compiled version of this header. */
    size_t kernelWordSize();

    /** Read subordinate register.  Some registers are wider than what can be easily represented on this architecture (e.g.,
     *  x86 XMM registers are 128 bits), therefore return the result as a bit vector. If you want just the low-order 64 bits,
     *  invoke it like this:
     *
     * @code
     *  uint64_t value = debugger.readRegister(RIP).toInteger();
     * @endcode */
    Sawyer::Container::BitVector readRegister(RegisterDescriptor);

    /** Read subordinate memory.
     *
     *  Returns the number of bytes read. The implementation accesses the subordinate memory via proc filesystem rather than
     *  sending PTRACE_PEEKDATA commands. This allows large areas of memory to be read efficiently. */
    size_t readMemory(rose_addr_t va, size_t nBytes, uint8_t *buffer);

    /** Returns true if the subordinate terminated. */
    bool isTerminated();

    /** String describing how the subordinate process terminated. */
    std::string howTerminated();

    /** Available registers. */
    const RegisterDictionary* registerDictionary() const;

    /** Disassembler. */
    Disassembler* disassembler() const {
        return disassembler_;
    }
    
    /** Returns the last status from a call to waitpid. */
    int waitpidStatus() const { return wstat_; }
    
private:
    // Initialize tables during construction
    void init();

    // Wait for subordinate or throw on error
    void waitForChild();

    // Open /dev/null with the specified flags as the indicated file descriptor, closing what was previously on that
    // descriptor. If an error occurs, the targetFd is closed anyway.
    void devNullTo(int targetFd, int openFlags);
};

std::ostream& operator<<(std::ostream&, const Debugger::Specimen&);

} // namespace
} // namespace

#endif
