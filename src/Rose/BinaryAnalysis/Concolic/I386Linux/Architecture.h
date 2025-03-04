#ifndef ROSE_BinaryAnalysis_Concolic_I386Linux_Architecture_H
#define ROSE_BinaryAnalysis_Concolic_I386Linux_Architecture_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/Yaml.h>

#include <Sawyer/Callbacks.h>

#include <boost/filesystem.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Linux Intel 80386 and compatible hardware architecture.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Features specific to native Linux ELF i386 specimens. */
class Architecture: public Concolic::Architecture {
    using Super = Concolic::Architecture;

public:
    /** Reference counting pointer. */
    using Ptr = ArchitecturePtr;

private:
    bool markingArgvAsInput_ = true;
    bool markingEnvpAsInput_ = false;

protected:
    Architecture(const std::string&);                   // for instantiating a factory
    Architecture(const DatabasePtr&, TestCaseId);
public:
    ~Architecture();

public:
    /** Allocating constructor for a factory. */
    static Ptr factory();

    /** Allocating constructor for test case.
     *
     *  Constructs a new executor for the specified test case.
     *
     * @{ */
    static Ptr instance(const DatabasePtr&, TestCaseId, const Yaml::Node &config);
    static Ptr instance(const DatabasePtr&, const TestCasePtr&, const Yaml::Node &config);
    /** @} */

public:
    // Same as super class, but casts result to a Linux debugger.
    Debugger::LinuxPtr debugger() const;

    /** The register where system call return values are stored. */
    RegisterDescriptor systemCallReturnRegister();

public:
    // These are documented in the base class.
    virtual bool matchFactory(const Yaml::Node&) const override;
    virtual Super::Ptr instanceFromFactory(const DatabasePtr&, TestCaseId, const Yaml::Node &config) const override;
    virtual Partitioner2::PartitionerPtr partition(const Partitioner2::EnginePtr&, const std::string &specimen) override;
    virtual void configureSystemCalls() override;
    virtual void configureSharedMemory(const Yaml::Node &config) override;
    virtual void load(const boost::filesystem::path&) override;
    virtual ByteOrder::Endianness memoryByteOrder() override;
    virtual std::vector<ExecutionEventPtr> createMemoryRestoreEvents() override;
    virtual std::vector<ExecutionEventPtr> createMemoryHashEvents() override;
    virtual std::vector<ExecutionEventPtr> createMemoryAdjustEvents(const MemoryMap::Ptr&, Address insnVa) override;
    virtual bool playEvent(const ExecutionEventPtr&) override;
    virtual void mapMemory(const AddressInterval&, unsigned permissions) override;
    virtual void unmapMemory(const AddressInterval&) override;
    virtual void createInputVariables(const Partitioner2::PartitionerConstPtr&, const Emulation::RiscOperatorsPtr&,
                                      const SmtSolver::Ptr &solver) override;
    virtual void systemCall(const Partitioner2::PartitionerConstPtr&,
                            const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;
    virtual void advanceExecution(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;
    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
        makeDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;

private:
    // Maps a scratch page for internal use and updates scratchVa_ with the address of the page.
    void mapScratchPage();

    // Copy (share) some data from the dst map to the src map at where. Generate map and write events that will map the memory
    // and initialize it when played back.
    std::vector<ExecutionEventPtr> copyMemory(const MemoryMap::Ptr &src, const MemoryMap::Ptr &dst, const AddressInterval &where,
                                              Address insnVa);

    // List of process memory segments that are not special.
    std::vector<MemoryMap::ProcessMapRecord> disposableMemory();

    // Unmap nearly all memory. The scratch page is not unmapped, nor is the VDSO or VVAR.
    void unmapAllMemory();

    // When stopped at the beginning of a system call, return the system call function number. See <sys/unistd_32.h> for the
    // mapping function system call number to the Linux kernel function that handles it.
    uint64_t systemCallFunctionNumber(const Partitioner2::PartitionerConstPtr&,
                                      const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    // Returns the system call argument.
    InstructionSemantics::BaseSemantics::SValuePtr
    systemCallArgument(const Partitioner2::PartitionerConstPtr&,
                       const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, size_t argNumber);

    // Returns the system call return value.
    InstructionSemantics::BaseSemantics::SValuePtr
    systemCallReturnValue(const Partitioner2::PartitionerConstPtr&,
                          const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    // Modify the symbolic system call return value.
    InstructionSemantics::BaseSemantics::SValuePtr
    systemCallReturnValue(const Partitioner2::PartitionerConstPtr&,
                          const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&,
                          const InstructionSemantics::BaseSemantics::SValuePtr&);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Linux i386 specific system call operations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for Linux i386 syscall callbacks.
 *
 *  This is the base class for all Linux i386 system call callbacks. */
class SyscallBase: public SyscallCallback {
    ExecutionEventPtr   latestReturnEvent_;             // event for most recent system call return
    ExecutionEventPtr   penultimateReturnEvent_;        // event for next most recent system call return

public:
    SyscallBase();
    virtual ~SyscallBase();

    /** Welcome message in diagnostic output.
     *
     *  Any @ref playback, @ref handlePreSyscall, and @ref handlePostSyscall methods defined in the child classes should
     *  call this function if they do anything. This causes tracing information to be emitted to the diagnostic output to
     *  aid in debugging. The @p name need not include the system call name since that will be added automatically. */
    void hello(const std::string &name, const SyscallContext&) const;

    /** Callback for syscall event playback.
     *
     *  This method, which must be implemented in subclasses, is invoked when a system call execution event is replayed. This
     *  occurs during the startup phase of concolic testing in order to bring the newly created concrete state up to the point
     *  it should be when the combined concrete plus symbolic phase takes over. */
    virtual void playback(SyscallContext&) = 0;

    /** Callback for new syscall events.
     *
     *  These two methods, @ref handlePreSyscall and @ref handlePostSyscall, are invoked after the startup phase (handled by
     *  @ref playback) each time a new system call is encountered. By time these are called, a system call @ref ExecutionEvent
     *  has already been created and represents the fact that the current instruction is a system call.
     *
     *  The @ref handlePreSyscall is invoked immediately before single-stepping the concrete process through the system call
     *  and is your opportunity to inspect the concrete state before the call occurs. If more than one callback is registered
     *  for a particular system call, then the @ref handlePreSyscall is only invoked for the first callback.  Since there's
     *  seldom pre-syscall work that needs to be done, subclasses do not have to override this function (it does nothing by
     *  default).
     *
     *  The @ref handlePostSyscall is invoked after single-stepping the concrete process through the system call. By this time,
     *  a provisional syscall return @ref ExecutionEvent and input variable has been created, and has been written to the
     *  symbolic state. Subclasses must implement this function, although they're allowed to make it a no-op.
     *
     * @{ */
    virtual void handlePreSyscall(SyscallContext&) {}
    virtual void handlePostSyscall(SyscallContext&) = 0;
    /** @} */

    /** Property: Prior system call return events.
     *
     *  All system calls that don't terminate the program return a value. Since these values are expected to be constrained in
     *  various ways by subclasses (such as a system call like @c getpid that always returns the same value, or @c time that
     *  normally returns monotonically increasing values), this base class tracks the return value events and makes them
     *  available through these two properties: @ref latestReturnEvent represents the return side effect for the most recent
     *  system call, and @ref penultimateReturnEvent represents the return side effect for the system call before that.
     *
     *  The shift of events happens at the beginning of processing a system call, before any subclass methods are
     *  invoked. During calls to the child class, the @ref penultimateReturnEvent is null when we're handling the first system
     *  call for this callback, and the @ref latestReturnEvent is null during the call to @ref handlePreSyscall since the current
     *  system call has not yet returned.
     *
     * @{ */
    ExecutionEventPtr latestReturnEvent() const;
    ExecutionEventPtr penultimateReturnEvent() const;
    /** @} */

    /** Emit information about the most recent return values.
     *
     *  This prints information about the @ref latestReturnEvent and @ref penultimateReturnEvent properties. The database argument
     *  is optional; if present then additional information such as event ID numbers are shown. */
    void showRecentReturnValues(std::ostream&, const SyscallContext&) const;

    /** Symbolic penultimate return value.
     *
     *  If the penultimate return event is associated with an input variable, then that input variable is returned. Otherwise
     *  the concrete return value is returned as a symbolic expression. If there is not a penultimate return event at all, then
     *  null is returned. */
    SymbolicExpressionPtr penultimateSymbolicReturn() const;

public:
    // Subclasses need not provide a function operator.
    virtual bool operator()(bool /*handled*/, SyscallContext&) override final;
};

/** Callbacks for system calls that are otherwise unimplemented.
 *
 *  This callback does nothing except create return value events, create return value input variables, and print an error
 *  message that the system call has not been implemented. It is not necessary for the user to create these objects and bind
 *  them to system calls -- that happens automatically if a system call has no other callbacks. */
class SyscallUnimplemented: public SyscallBase {
protected:
    SyscallUnimplemented();
public:
    ~SyscallUnimplemented();

public:
    /** Allocating constructor. */
    static Ptr instance();

    void playback(SyscallContext&) override;
    void handlePostSyscall(SyscallContext&) override;
};

/** Callback for system calls that return an input.
 *
 *  This is for system calls that return a value that is treated as an input variable, but don't have any other constraints
 *  on their behavior. */
class SyscallReturnsInput: public SyscallBase {
protected:
    SyscallReturnsInput();
public:
    ~SyscallReturnsInput();

public:
    /** Allocating constructor. */
    static Ptr instance();

    void playback(SyscallContext&) override;
    void handlePostSyscall(SyscallContext&) override;
};

/** Callback for system calls that terminate the process. */
class SyscallTerminates: public SyscallBase {
protected:
    SyscallTerminates();
public:
    ~SyscallTerminates();

public:
    /** Allocating constructor. */
    static Ptr instance();

    void playback(SyscallContext&) override;
    void handlePostSyscall(SyscallContext&) override;
};

/** Base class for constraints on return values.
 *
 *  Subclasses must implement the @ref playback and @ref makeReturnConstraint methods. */
class SyscallReturn: public SyscallBase {
protected:
    SyscallReturn();
public:
    ~SyscallReturn();

protected:
    /** Creates the symbolic constraint and concrete return value.
     *
     *  The symbolic constraint is a Boolean expression that is added to the SMT solver. The concrete return value
     *  must satisfy the symbolic constraint. Either or both of the return values may be missing.  If no symbolic constraint is
     *  returned, then none will be added to the solver. If no concrete value is returned, then the conrete state of the
     *  test case will not be updated (it will use whatever concrete value the syscall actually returned). */
    virtual std::pair<SymbolicExpressionPtr, Sawyer::Optional<uint64_t>> makeReturnConstraint(SyscallContext&) = 0;

public:
    void handlePostSyscall(SyscallContext&) override final;
};

/** Callback for system calls that return a constant.
 *
 *  This callback can be used for system calls like @c getpid that always return the same value no matter how many times
 *  they're called. Note that although the return value is constant within a particular test case, it is not necessarily
 *  constant across all test cases (unless solver constraints require it to be so). */
class SyscallConstant: public SyscallReturn {
protected:
    SyscallConstant();
public:
    ~SyscallConstant();

public:
    /** Allocating constructor. */
    static Ptr instance();

    void playback(SyscallContext&) override;
    std::pair<SymbolicExpressionPtr, Sawyer::Optional<uint64_t>> makeReturnConstraint(SyscallContext&) override;
};

/** Callback for system calls that return non-decreasing values.
 *
 *  This callback can be used for system calls like @c time that return non-decreasing values. */
class SyscallNondecreasing: public SyscallReturn {
protected:
    SyscallNondecreasing();
public:
    ~SyscallNondecreasing();

public:
    /** Allocating constructor. */
    static Ptr instance();

    void playback(SyscallContext&) override;
    std::pair<SymbolicExpressionPtr, Sawyer::Optional<uint64_t>> makeReturnConstraint(SyscallContext&) override;
};

/** Callback for access system call. */
class SyscallAccess: public SyscallBase {
protected:
    SyscallAccess();
public:
    ~SyscallAccess();

public:
    /** Allocating constructor. */
    static Ptr instance();

    void playback(SyscallContext&) override;
    void handlePostSyscall(SyscallContext&) override;
};

/** Callback for brk system call. */
class SyscallBrk: public SyscallBase {
protected:
    SyscallBrk();
public:
    ~SyscallBrk();

public:
    /** Allocating constructor. */
    static Ptr instance();

    void playback(SyscallContext&) override;
    void handlePostSyscall(SyscallContext&) override;
};

/** Callback for mmap2 system call. */
class SyscallMmap2: public SyscallBase {
protected:
    SyscallMmap2();
public:
    ~SyscallMmap2();

public:
    /** Allocating constructor. */
    static Ptr instance();

    void playback(SyscallContext&) override;
    void handlePostSyscall(SyscallContext&) override;
};

/** Callback for openat system call. */
class SyscallOpenat: public SyscallBase {
protected:
    SyscallOpenat();
public:
    ~SyscallOpenat();

public:
    /** Allocating constructor. */
    static Ptr instance();

    void playback(SyscallContext&) override;
    void handlePostSyscall(SyscallContext&) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
