#ifndef ROSE_BinaryAnalysis_Debugger_H
#define ROSE_BinaryAnalysis_Debugger_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BitFlags.h>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
#include <boost/regex.hpp>
#include <Disassembler.h>
#include <Sawyer/BitVector.h>
#include <Sawyer/Message.h>
#include <Sawyer/Optional.h>
#include <Sawyer/Trace.h>

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

    /** How to detach from a process when the debugger is destroyed. */
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
        unsigned long persona_;                         // personality(2) flags

        // Members for running a program
        boost::filesystem::path program_;               // full path of executable program
        std::vector<std::string> arguments_;            // command-line arguments of executable program
        boost::filesystem::path workingDirectory_;      // name or working directory, or use CWD if empty
        std::vector<boost::regex> clearEnvVars_;        // clear environment variables matching these patterns
        std::map<std::string, std::string> setEnvVars_; // environment variables to be set

        // Members for attaching to a process
        int pid_;                                       // process ID (int instead of pid_t for portability)

    public:
        /** Default construct an empty specimen descriptor. */
        Specimen()
            : persona_(getPersonality()), pid_(-1) {}

        /** Construct a specimen description for a process. */
        Specimen(int pid) /*implicit*/
            : flags_(DEFAULT_FLAGS), persona_(getPersonality()), pid_(pid) {}

        /** Construct a specimen description for a program with no arguments. */
        Specimen(const boost::filesystem::path &name) /*implicit*/
            : flags_(DEFAULT_FLAGS), persona_(getPersonality()), program_(name), pid_(-1) {}

        /** Construct a specimen description for a program with arguments. */
        Specimen(const boost::filesystem::path &name, const std::vector<std::string> &args)
            : flags_(DEFAULT_FLAGS), persona_(getPersonality()), program_(name), arguments_(args), pid_(-1) {}

        /** Construct a specimen description from combined program and arguments. */
        Specimen(const std::vector<std::string> &nameAndArgs) /*implicit*/
            : flags_(DEFAULT_FLAGS), persona_(getPersonality()), program_(nameAndArgs.front()),
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

        /** Remove an environment variable.
         *
         *  The specified environment variable is removed from environment before starting the subordinate process. This
         *  function can be called multiple times to remove multiple variables. The variables are removed from the subordinate
         *  process without affecting process from which this is called. Removals happen before additions. */
        void eraseEnvironmentVariable(const std::string&);

        /** Remove some environment variables.
         *
         *  Variables whose names match the specified regular expression are removed from the environment before starting the
         *  subordinate process. This function can be called multiple times to remove multiple variables. The variables are
         *  removed from the subordinate process without affectig the calling process. Removals happen before additions. */
        void eraseMatchingEnvironmentVariables(const boost::regex&);

        /** Remove all environment variables.
         *
         *  All environment variables are removed before starting the subordinate process. Removals happen before additions. */
        void eraseAllEnvironmentVariables();

        /** Add an environment variable.
         *
         *  The specified variable and value is added to the subordinate process's environment. This function can be called
         *  multiple times to add multiple variables. The variables are added to the subordinate process without affecting this
         *  calling process. Additions happen after removals. */
        void insertEnvironmentVariable(const std::string &name, const std::string &value);

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

        /** Property: Personality flags.
         *
         *  These flags are identical to the bit flags used by the Linux @c personality function. For instance, to turn off
         *  address space randomization, include sys/personality.h and pass @c ADDR_NO_RANDOMIZE. See also, @ref
         *  randomizedAddresses property.
         *
         * @{ */
        unsigned long persona() const {
            return persona_;
        }
        void persona(unsigned long bits) {
            persona_ = bits;
        }
        /** @} */

        /** Property: Whether to randomize addresses of a process.
         *
         *  This is actually a @ref persona property, but it's used so commonly that we have a separate API for turning it
         *  on and off. The alleviates the user from having to test what kind of machine he's compiling on and using conditional
         *  compilation to include the correct files and use the correct constants.  If the host machine doesn't support adjusting
         *  whether address space randomization is used, then setting this property is a no-op and false is always returned.
         *
         * @{ */
        bool randomizedAddresses() const;
        void randomizedAddresses(bool);
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

        // Used internally.
        char** prepareEnvAdjustments() const;
    };

public:
    static Sawyer::Message::Facility mlog;              /**< Diagnostic facility for debugger. */

private:
    typedef Sawyer::Container::Map<RegisterDescriptor, size_t> UserRegDefs;
    enum RegPageStatus { REGPAGE_NONE, REGPAGE_REGS, REGPAGE_FPREGS };

    Specimen specimen_;                                 // description of specimen being debugged
    int child_;                                         // process being debugged (int, not pid_t, for Windows portability)
    DetachMode autoDetach_;                             // how to detach from the subordinate when deleting this debugger
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
        : child_(0), autoDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE),
          disassembler_(NULL) {
        init();
    }

    /** Construct a debugger attached to a specimen. */
    explicit Debugger(const Specimen &specimen)
        : child_(0), autoDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE),
          disassembler_(NULL) {
        init();
        attach(specimen);
    }

public:
    ~Debugger() {
        detach(autoDetach_);
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
    /** Attach to a specimen.
     *
     *  The @p onDelete argument specifies what to do with the subordinate process if this debugger object is deleted
     *  while the subordinate is still attached. If @p onDelete is not specified, then a reasonable value is chosen: for
     *  subordinates that existed prior to attaching (e.g., Linux process ID), the default detach mechanism is either @ref
     *  DETACH or @ref NOTHING depending on whether the @ref Specimen ATTACH flag was set or clear. For subordniates that
     *  are created by this @ref attach method (such as ELF executables), the detach mechanism is @ref KILL. */
    void attach(const Specimen&, Sawyer::Optional<DetachMode> onDelete = Sawyer::Nothing());
    /** @} */

    /** Returns true if attached to a subordinate.  Return value is the subordinate process ID. */
    int isAttached() { return child_; }

    /** Detach from the subordinate.
     *
     *  If a detach mode is specified then use that mechanism to detatch. Otherwise use whatever mechanism was chosen as
     *  the default during the @ref attach operation. */
    void detach(Sawyer::Optional<DetachMode> mode = Sawyer::Nothing());

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

    /** Run the program and return an execution trace. */
    Sawyer::Container::Trace<rose_addr_t> trace();

    /** Action for trace filter callback. */
    enum FilterActionFlags {
        REJECT = 0x00000001,                          /**< Reject the current address, not appending it to the trace. */
        STOP   = 0x00000002                           /**< Abort tracing, either appending or rejecting the current address. */
    };

    /** Return value for tracing.
     *
     *  The return value from the trace filter indicates whether the current address should be appended to the trace or
     *  rejected, and whether the tracing operation should continue or stop.  A default constructed @ref FilterAction will
     *  append the current address to the trace and continue tracing, which is normally what one wants. */
    typedef BitFlags<FilterActionFlags> FilterAction;

    /** Run the program and return an execution trace.
     *
     *  At each step along the execution, the @p filter functor is invoked and passed the current execution address. The return
     *  value of type @ref FilterAction from the filter functor controls whether the address is appended to the trace and whether the
     *  tracing should continue. */
    template<class Filter>
    Sawyer::Container::Trace<rose_addr_t> trace(Filter &filter) {
        Sawyer::Container::Trace<rose_addr_t> retval;
        while (!isTerminated()) {
            rose_addr_t va = executionAddress();
            FilterAction action = filter(va);
            if (action.isClear(REJECT))
                retval.append(va);
            if (action.isSet(STOP))
                return retval;
            singleStep();
        }
        return retval;
    }

    /** Obtain and cache kernel's word size in bits.  The wordsize of the kernel is not necessarily the same as the word size
     * of the compiled version of this header. */
    size_t kernelWordSize();

    /** Read subordinate register.
     *
     *  Some registers are wider than what can be easily represented on this architecture (e.g., x86 XMM registers are 128
     *  bits), therefore return the result as a bit vector. If you want just the low-order 64 bits, invoke it like this:
     *
     * @code
     *  uint64_t value = debugger->readRegister(RIP).toInteger();
     * @endcode */
    Sawyer::Container::BitVector readRegister(RegisterDescriptor);

    /** Write subordinate register.
     *
     *  @{ */
    void writeRegister(RegisterDescriptor, const Sawyer::Container::BitVector&);
    void writeRegister(RegisterDescriptor, uint64_t value);
    /** @} */

    /** Read subordinate memory as a bit vector.
     *
     * @code
     *  uint64_t value = debugger->readMemory(0x12345678, 4, ByteOrder::ORDER_LSB).toInteger();
     * @endcode */
    Sawyer::Container::BitVector readMemory(rose_addr_t va, size_t nBytes, ByteOrder::Endianness order);

    /** Writes some bytes to subordinate memory.
     *
     *  Returns the number of bytes written. */
    size_t writeMemory(rose_addr_t va, size_t nBytes, const uint8_t *bytes);

    /** Write subordinate memory.
     *
     *  Writes something to memory. */
    template<typename T>
    void writeMemory(rose_addr_t va, const T &value) {
        size_t n = writeMemory(va, sizeof(T), (const uint8_t*)&value);
        ASSERT_always_require(n == sizeof(T));
    }

    /** Read subordinate memory.
     *
     *  Returns the number of bytes read. The implementation accesses the subordinate memory via proc filesystem rather than
     *  sending PTRACE_PEEKDATA commands. This allows large areas of memory to be read efficiently. */
    size_t readMemory(rose_addr_t va, size_t nBytes, uint8_t *buffer);

    /** Read C-style NUL-terminated string from subordinate.
     *
     *  Reads up to @p maxBytes bytes from the subordinate or until an ASCII NUL character is read, concatenates all the
     *  characters (except the NUL) into a C++ string and returns it. The @p maxBytes includes the NUL terminator although the
     *  NUL terminator is not returned as part of the string. */
    std::string readCString(rose_addr_t va, size_t maxBytes = UNLIMITED);

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

public:
    /**  Initialize diagnostic output. This is called automatically when ROSE is initialized.  */
    static void initDiagnostics();

private:
    // Initialize tables during construction
    void init();

    // Wait for subordinate or throw on error
    void waitForChild();

    // Open /dev/null with the specified flags as the indicated file descriptor, closing what was previously on that
    // descriptor. If an error occurs, the targetFd is closed anyway.
    void devNullTo(int targetFd, int openFlags);

    // Get/set personality in a portable way
    static unsigned long getPersonality();
    static void setPersonality(unsigned long);
};

std::ostream& operator<<(std::ostream&, const Debugger::Specimen&);

} // namespace
} // namespace

#endif
#endif
