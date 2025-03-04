#ifndef ROSE_BinaryAnalysis_Debugger_Linux_H
#define ROSE_BinaryAnalysis_Debugger_Linux_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_LINUX
#include <Rose/BinaryAnalysis/Debugger/Base.h>

#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <Rose/BinaryAnalysis/SystemCall.h>

#include <Sawyer/Optional.h>
#include <sys/ptrace.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

/** Debugger based on Linux ptrace. */
class Linux: public Base {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Shared ownership pointer. */
    using Ptr = LinuxPtr;

    /** How to detach from a process when the debugger is destroyed. */
    enum class DetachMode {
        KILL,                                           /**< Kill the process. */
        DETACH,                                         /**< Simply detach leaving process in current state. */
        CONTINUE,                                       /**< Detach from process and cause it to continue running. */
        NOTHING                                         /**< Do nothing. */
    };

    /** Flags controlling operation. */
    enum class Flag {
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
        int pid_ = -1;                                  // process ID (int instead of pid_t for portability)

    public:
        /** Default construct an empty specimen descriptor. */
        Specimen();

        /** Construct a specimen description for a process. */
        Specimen(int pid);                              // implicit

        /** Construct a specimen description for a program with no arguments. */
        Specimen(const boost::filesystem::path&);       // implicit

        /** Construct a specimen description for a program with arguments. */
        Specimen(const boost::filesystem::path &name, const std::vector<std::string> &args);

        /** Construct a specimen description from combined program and arguments. */
        Specimen(const std::vector<std::string> &nameAndArgs); // implicit

    public:
        /** Property: Name of executable program to run.
         *
         *  A specimen description can be either a program or a process, but not both. Setting this property will
         *  clear the @ref process property.
         *
         * @{ */
        const boost::filesystem::path& program() const;
        void program(const boost::filesystem::path&);
        /** @} */

        /** Property: Program command-line arguments.
         *
         *  This property is the list of command-line arguments supplied to a program when starting it. This property is
         *  not used when attaching to an existing process.
         *
         * @{ */
        const std::vector<std::string>& arguments() const;
        void arguments(const std::vector<std::string>&);
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
        boost::filesystem::path workingDirectory() const;
        void workingDirectory(const boost::filesystem::path&);
        /** @} */

        /** Property: Operational flags.
         *
         *  These flags control some finer aspects of the operations.
         *
         * @{ */
        const BitFlags<Flag>& flags() const;
        BitFlags<Flag>& flags();
        /** @} */

        /** Property: Personality flags.
         *
         *  These flags are identical to the bit flags used by the Linux @c personality function. For instance, to turn off
         *  address space randomization, include sys/personality.h and pass @c ADDR_NO_RANDOMIZE. See also, @ref
         *  randomizedAddresses property.
         *
         * @{ */
        unsigned long persona() const;
        void persona(unsigned long bits);
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
        int process() const;
        void process(int pid);
        /** @} */

        /** Print some basic info about the specimen. */
        void print(std::ostream &out) const;

        // Used internally.
        char** prepareEnvAdjustments() const;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Private types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    using UserRegDefs = Sawyer::Container::Map<RegisterDescriptor, size_t>;

    // What type of register values are cached?
    enum class RegCacheType { NONE, REGS, FPREGS };

    // Low-level collection of register values.
    using RegPage = std::array<uint8_t, 512>;

    // Low-level structure containing values for all registers.
    struct AllRegValues {
        RegPage regs;                                   // integer registers
        RegPage fpregs;                                 // floating-point registers
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    Specimen specimen_;                                 // description of specimen being debugged
    int child_ = 0;                                     // process being debugged (int, not pid_t, for Windows portability)
    DetachMode autoDetach_ = DetachMode::KILL;          // how to detach from the subordinate when deleting this debugger
    int wstat_ = -1;                                    // last status from waitpid
    AddressIntervalSet breakPoints_;                    // list of break point addresses
    int sendSignal_ = 0;                                // pending signal
    UserRegDefs userRegDefs_;                           // how registers map to user_regs_struct in <sys/user.h>
    UserRegDefs userFpRegDefs_;                         // how registers map to user_fpregs_struct in <sys/user.h>
    size_t kernelWordSize_ = 0;                         // cached width in bits of kernel's words
    RegPage regCache_;                                  // latest register information read from subordinate
    RegCacheType regCacheType_ = RegCacheType::NONE;    // what are the contents of regsPage_?
    Sawyer::Optional<Address> syscallVa_;               // address of some executable system call instruction.
    SystemCall syscallDecls_;                           // to get declarations for system calls

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initialization and destruction.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Allocating constructor to create an empty debugger.
     *
     *  The returned debugger is not attached to any specimen. */
    static Ptr instance();

    /** Allocating constructor that attaches to a specimen. */
    static Ptr instance(const Specimen &specimen, Sawyer::Optional<DetachMode> onDelete = Sawyer::Nothing());

    Linux();
    ~Linux();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Attaching and detaching
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Attach to a specimen.
     *
     *  The @p onDelete argument specifies what to do with the subordinate process if this debugger object is deleted while the
     *  subordinate is still attached. If @p onDelete is not specified, then a reasonable value is chosen: for subordinates that
     *  existed prior to attaching (e.g., Linux process ID), the default detach mechanism is either @ref DetachMode::DETACH or @ref
     *  DetachMode::NOTHING depending on whether the @ref Specimen ATTACH flag was set or clear. For subordniates that are created
     *  by this @ref attach method (such as ELF executables), the detach mechanism is @ref DetachMode::KILL. */
    void attach(const Specimen&, Sawyer::Optional<DetachMode> onDelete = Sawyer::Nothing());

    /** Process ID of attached process. */
    Sawyer::Optional<int> processId() const;

    /** Property: How to detach from the specimen.
     *
     *  When the @ref detach method is called, this property determines how the specimen is detached.
     *
     * @{ */
    DetachMode detachMode() const;
    void detachMode(DetachMode);
    /** @} */

    /** Obtain and cache the kernel word size in bits.
     *
     *  The word size of the kernel is not necesarily the same as the word size of the compiled version of this header. */
    size_t kernelWordSize();

    /** Returns the last status from a call to waitpid. */
    int waitpidStatus() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Running
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Execute to a system call. */
    void stepIntoSystemCall(ThreadId);

    /** Run until the next system call.
     *
     *  The subordinate is run until it is about to make a system call or has just returned from a system call, or it has
     *  encountered a signal or terminated. Execution does not stop at break points. */
    void runToSystemCall(ThreadId);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // System calls
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Cause the subordinate to execute a system call.
     *
     *  @{ */
    int64_t remoteSystemCall(ThreadId, int syscallNumber);
    int64_t remoteSystemCall(ThreadId, int syscallNumber,
                             uint64_t arg1);
    int64_t remoteSystemCall(ThreadId, int syscallNumber,
                             uint64_t arg1, uint64_t arg2);
    int64_t remoteSystemCall(ThreadId, int syscallNumber,
                             uint64_t arg1, uint64_t arg2, uint64_t arg3);
    int64_t remoteSystemCall(ThreadId, int syscallNumber,
                             uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);
    int64_t remoteSystemCall(ThreadId, int syscallNumber,
                             uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);
    int64_t remoteSystemCall(ThreadId, int syscallNumber,
                             uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6);
    int64_t remoteSystemCall(ThreadId, int syscallNumber, std::vector<uint64_t> args);
    /** @} */

    /** Cause the subordinate to open a file.
     *
     *  The subordinate will be coerced into opening the specified file, and the file descriptor in the subordinate is
     *  returned. */
    int remoteOpenFile(ThreadId, const boost::filesystem::path &fileName, unsigned flags, mode_t mode);

    /** Cause the subordinate to close a file. */
    int remoteCloseFile(ThreadId, unsigned remoteFd);

    /** Map a new memory region in the subordinate.
     *
     *  This is similar to the @c mmap function, except it operates on the subordinate process. */
    Address remoteMmap(ThreadId, Address va, size_t nBytes, unsigned prot, unsigned flags, const boost::filesystem::path&,
                       off_t offset);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides for methods declared and documented in the super class.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual bool isAttached() override;
    virtual void detach() override;
    virtual void terminate() override;
    virtual std::vector<ThreadId> threadIds() override;
    virtual void executionAddress(ThreadId, Address) override;
    virtual Address executionAddress(ThreadId) override;
    virtual void setBreakPoint(const AddressInterval&) override;
    virtual void clearBreakPoint(const AddressInterval&) override;
    virtual void clearBreakPoints() override;
    virtual void singleStep(ThreadId) override;
    virtual void runToBreakPoint(ThreadId) override;
    virtual Sawyer::Container::BitVector readRegister(ThreadId, RegisterDescriptor) override;
    virtual void writeRegister(ThreadId, RegisterDescriptor, const Sawyer::Container::BitVector&) override;
    virtual void writeRegister(ThreadId, RegisterDescriptor, uint64_t value) override;
    virtual size_t readMemory(Address va, size_t nBytes, uint8_t *buffer) override;
    virtual std::vector<uint8_t> readMemory(Address va, size_t nBytes) override;
    virtual Sawyer::Container::BitVector readMemory(Address va, size_t nBytes, ByteOrder::Endianness order) override;
    virtual size_t writeMemory(Address va, size_t nBytes, const uint8_t *bytes) override;
    virtual bool isTerminated() override;
    virtual std::string howTerminated() override;
    virtual std::vector<RegisterDescriptor> availableRegisters() override;
    virtual Sawyer::Container::BitVector readAllRegisters(ThreadId) override;
    virtual void writeAllRegisters(ThreadId, const Sawyer::Container::BitVector&) override;

private:
    // Wait for subordinate or throw on error
    void waitForChild();

    // Open /dev/null with the specified flags as the indicated file descriptor, closing what was previously on that
    // descriptor. If an error occurs, the targetFd is closed anyway.
    void devNullTo(int targetFd, int openFlags);

    // Get/set personality in a portable way
    static unsigned long getPersonality();
    static void setPersonality(unsigned long);

    // Address of a system call instruction. The initial search can be expensive, so the result is cached.
    Sawyer::Optional<Address> findSystemCall();

    // Low-level methods to read and write data for all known registers.
    AllRegValues loadAllRegisters(ThreadId);
    void saveAllRegisters(ThreadId, const AllRegValues&);

    // Update the register cache to contain the specified register. Return the offset to the start of the register
    // inside the register cache.
    size_t updateRegCache(RegisterDescriptor);

    // Send a ptrace command
    long sendCommand(__ptrace_request, void *addr = nullptr, void *data = nullptr);
    long sendCommandInt(__ptrace_request, void *addr, int i);

    // Load system call declarations from the appropriate header file
    void declareSystemCalls(size_t nBits);
};

std::ostream& operator<<(std::ostream&, const Linux::Specimen&);

} // namespace
} // namespace
} // namespace

#endif
#endif
