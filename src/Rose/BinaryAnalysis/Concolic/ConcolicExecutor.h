#ifndef ROSE_BinaryAnalysis_Concolic_ConcolicExecutor_H
#define ROSE_BinaryAnalysis_Concolic_ConcolicExecutor_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Sawyer/FileSystem.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Concolic emulation semantics.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Semantics for concolic execution. */
namespace Emulation {

typedef InstructionSemantics::SymbolicSemantics::Formatter SValueFormatter; /**< How to format expressions when printing. */
typedef InstructionSemantics::SymbolicSemantics::SValuePtr SValuePtr; /**< Pointer to semantic values. */
typedef InstructionSemantics::SymbolicSemantics::SValue SValue; /**< Type of semantic values. */
typedef InstructionSemantics::SymbolicSemantics::RegisterStatePtr RegisterStatePtr; /**< Pointer to semantic registers. */
typedef InstructionSemantics::SymbolicSemantics::RegisterState RegisterState; /**< Type of semantic register space. */
typedef InstructionSemantics::SymbolicSemantics::MemoryStatePtr MemoryStatePtr; /**< Pointer to semantic memory. */
typedef InstructionSemantics::SymbolicSemantics::MemoryState MemoryState; /**< Type of semantic memory space. */
typedef InstructionSemantics::SymbolicSemantics::StatePtr StatePtr; /**< Pointer to semantic machine state. */
typedef InstructionSemantics::SymbolicSemantics::State State; /**< Semantic machine state. */

/** Values thrown when subordinate exits. */
class Exit: public Exception {
    uint64_t status_;

public:
    explicit Exit(uint64_t status)
        : Exception("subordinate exit"), status_(status) {}

    ~Exit() throw () {}

    /** Symbolic exit status. */
    uint64_t status() const {
        return status_;
    }
};

/** Semantic operations. */
class RiscOperators: public InstructionSemantics::SymbolicSemantics::RiscOperators {
public:
    /** Shared ownership pointer. */
    using Ptr = RiscOperatorsPtr;

    /** Base class. */
    typedef InstructionSemantics::SymbolicSemantics::RiscOperators Super;

    /** Special "path" Boolean register. */
    const RegisterDescriptor REG_PATH;

    /** Settings for the emulation. */
    struct Settings {
    };

private:
    Settings settings_;                                 // emulation settings
    DatabasePtr db_;                                    // concolic database connection
    TestCasePtr testCase_;                              // test case whose instructions are being processed
    const Partitioner2::Partitioner &partitioner_;      // ROSE disassembly info about the specimen
    ArchitecturePtr process_;                           // subordinate process, concrete state
    bool hadSystemCall_ = false;                        // true if we need to call process_->systemCall, cleared each insn
    ExecutionEventPtr hadSharedMemoryAccess_;           // set when shared memory is read, cleared each instruction
    bool isRecursive_ = false;                          // if set, avoid calling user-defined functions

protected:
    /** Allocating constructor. */
    RiscOperators(const Settings&, const DatabasePtr&, const TestCasePtr&, const Partitioner2::Partitioner&,
                  const ArchitecturePtr&, const InstructionSemantics::BaseSemantics::StatePtr&, const SmtSolverPtr&);
public:
    ~RiscOperators();

public:
    /** Allocating constructor. */
    static RiscOperatorsPtr instance(const Settings &settings, const DatabasePtr&, const TestCasePtr&,
                                     const Partitioner2::Partitioner&, const ArchitecturePtr &process,
                                     const InstructionSemantics::BaseSemantics::SValuePtr &protoval,
                                     const SmtSolverPtr &solver = SmtSolverPtr());

    /** Dynamic pointer downcast. */
    static RiscOperatorsPtr promote(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    // Overrides documented in base class
    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics::BaseSemantics::SValuePtr &/*protoval*/,
           const SmtSolverPtr& = SmtSolverPtr()) const override;
    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics::BaseSemantics::StatePtr &/*state*/,
           const SmtSolverPtr& = SmtSolverPtr()) const override;

public:
    /** Property: Settings.
     *
     *  The settings are read-only, set when this object was created. */
    const Settings& settings() const;

    /** Property: Partitioner. */
    const Partitioner2::Partitioner& partitioner() const;

    /** Property: Test case. */
    TestCasePtr testCase() const;

    /** Property: Database. */
    DatabasePtr database() const;

    /** Property: Concrete half of the concolic executor semantics. */
    ArchitecturePtr process() const;

    /** Property: Input variables.
     *
     *  This is a mapping from symbolic variables to execution events.
     *
     * @{ */
    InputVariablesPtr inputVariables() const;
    /** @} */

    /** Property: Had system call.
     *
     *  Cleared at the beginning of each instruction, and set when processing an instruction symbolically that is a system
     *  call. This allows the system call handling to be delayed until after the concrete half of the execution is performed,
     *  namely stepping into the system call but not yet performing it.
     *
     * @{ */
    bool hadSystemCall() const;
    void hadSystemCall(bool);
    /** @} */

    /** Property: Had a shared memory access.
     *
     *  Cleared at the beginning of each instruction, and set when processing an instruction symbolically that accesses known
     *  shared memory.
     *
     * @{ */
    ExecutionEventPtr hadSharedMemoryAccess() const;
    void hadSharedMemoryAccess(const ExecutionEventPtr&);
    /** @} */

    /** Property: Recursive calls through user code.
     *
     *  This is generally tested and set before invoking user-defined code such as callbacks for shared memory operations, and cleared
     *  when the user code returns. If the test is true, then the user code is skipped.
     *
     * @{ */
    bool isRecursive() const;
    void isRecursive(bool);
    /** @} */

    /** Number of bits in a word.
     *
     *  The definition of "word" is the natural width of the instruction pointer, stack pointer, most general-purpose
     *  registers, etc. */
    size_t wordSizeBits() const;

    /** Register definitions. */
    RegisterDictionaryPtr registerDictionary() const;

    /** Create and save info about initial program inputs.
     *
     *  This function creates the execution events and symbolic variables for program arguments (argc, argv, envp) and creates
     *  a mapping from the symbolic variable to its corresponding concrete execution event, and saves the information in
     *  the database. */
    void createInputVariables(const SmtSolver::Ptr&);

    /** Restore saved input variables.
     *
     *  Reads the database to reconstruct information about program inputs. */
    void restoreInputVariables(const SmtSolver::Ptr&);

    /** Print input variables.
     *
     *  Shows the mapping from input variables to their symbolic values. */
    void printInputVariables(std::ostream&) const;

    /** Print SMT solver assertions. */
    void printAssertions(std::ostream&) const;

public:
    // Base class overrides -- the acutal RISC operations

    virtual void startInstruction(SgAsmInstruction*) override;

    virtual void interrupt(int majr, int minr) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    readRegister(RegisterDescriptor reg, const InstructionSemantics::BaseSemantics::SValuePtr &dflt) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    readRegister(RegisterDescriptor reg) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    peekRegister(RegisterDescriptor reg, const InstructionSemantics::BaseSemantics::SValuePtr &dflt) override;

    virtual void
    writeRegister(RegisterDescriptor, const InstructionSemantics::BaseSemantics::SValuePtr&) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    readMemory(RegisterDescriptor segreg, const InstructionSemantics::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics::BaseSemantics::SValuePtr &dflt,
               const InstructionSemantics::BaseSemantics::SValuePtr &cond) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    peekMemory(RegisterDescriptor segreg, const InstructionSemantics::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics::BaseSemantics::SValuePtr &dflt) override;

    // Call this when the concrete simulation exits.
    void doExit(uint64_t);
};

/**< Pointer to virtual CPU. */
typedef boost::shared_ptr<class Dispatcher> DispatcherPtr;

/** CPU for concolic emulation. */
class Dispatcher: public InstructionSemantics::DispatcherX86 {
    using Super = InstructionSemantics::DispatcherX86;

public:
    /** Shared ownership pointer. */
    using Ptr = boost::shared_ptr<Dispatcher>;

protected:
    /** Constructor. */
    explicit Dispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);
public:
    ~Dispatcher();

public:
    /** Allocating constructor. */
    static DispatcherPtr instance(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

public:
    /** Concrete instruction pointer. */
    rose_addr_t concreteInstructionPointer() const;

    /** True if subordinate process has terminated.
     *
     *  Once the subordinate process terminates no more instructions can be processed and no state information
     *  is available. */
    bool isTerminated() const;

    /** Return the emulation RISC operators.
     *
     *  Returns the @ref Emulation::RiscOperators object even if this dispatcher's immediate RISC operators object is a
     *  TraceSemantics::RiscOperators being used for debugging purposes. */
    RiscOperatorsPtr emulationOperators() const;

    /** Unrwap the RISC operators if tracing is enabled. */
    static RiscOperatorsPtr unwrapEmulationOperators(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    /** The concrete half of processInstruction. */
    void processConcreteInstruction(SgAsmInstruction*);

public:
    // overrides
    virtual void processInstruction(SgAsmInstruction*) override;
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Concolic executor.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Concolic executor.
 *
 *  Executes a test case both concretely and symbolically and generates new test cases. */
class ConcolicExecutor: public Sawyer::SharedObject {
public:
    /** Reference counting pointer to @ref ConcolicExecutor. */
    typedef Sawyer::SharedPointer<ConcolicExecutor> Ptr;

    /** Settings to control various aspects of an executor. */
    struct Settings {
        Partitioner2::EngineSettings partitionerEngine;
        Partitioner2::LoaderSettings loader;
        Partitioner2::DisassemblerSettings disassembler;
        Partitioner2::PartitionerSettings partitioner;
        Emulation::RiscOperators::Settings emulationSettings;

        bool traceSemantics;                            /** Whether to debug semantic steps by using a semantic tracer. */
        AddressIntervalSet showingStates;               /** Instructions after which to show the semantic state. */

        Settings()
            : traceSemantics(false) {}
    };

    /** Information about a called function. */
    struct FunctionCall {
        std::string printableName;                      /** Name suitable for printing in diagnostic messages. */
        rose_addr_t sourceVa;                           /** Address from which the function was called. */
        rose_addr_t targetVa;                           /** Address that was called. */
        rose_addr_t stackVa;                            /** Stack pointer when function is first called. */

        FunctionCall()
            : sourceVa(0), targetVa(0), stackVa(0) {}

        FunctionCall(const std::string &printableName, rose_addr_t sourceVa, rose_addr_t targetVa, rose_addr_t stackVa)
            : printableName(printableName), sourceVa(sourceVa), targetVa(targetVa), stackVa(stackVa) {}
    };

private:
    Settings settings_;
    std::vector<FunctionCall> functionCallStack_;
    Sawyer::FileSystem::TemporaryDirectory tmpDir_;

    // These can be configured by the user before calling configureExecution.
    SmtSolverPtr solver_;                               // solver used during execution

    // These are initialized by configureExecution
    TestCasePtr testCase_;                              // currently executing test case
    TestCaseId testCaseId_;                             // database ID for currently executing test case
    DatabasePtr db_;                                    // database for all this stuff
    Partitioner2::Partitioner partitioner_;             // used during execution
    ArchitecturePtr process_;                           // the concrete half of the execution
    Emulation::DispatcherPtr cpu_;                      // the symbolic half of the execution

protected:
    ConcolicExecutor();

public:
    ~ConcolicExecutor();

public:
    /** Allcoating constructor. */
    static Ptr instance();

    /** Property: Configuration settings.
     *
     *  These settings control the finer aspects of this @ref ConcolicExecutor. They should generally be set immediately
     *  after construction this executor and before any operations are invoked that might use the settings.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    const Settings& settings() const;
    Settings& settings();
    /** @} */

    /** Property: SMT solver to use during execution.
     *
     *  The solver property can be initialized by the user before concolic execution starts. If the user does not set this
     *  property, then a solver will be created when execution starts. This property should not be modified after execution
     *  starts.
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    SmtSolverPtr solver() const;
    void solver(const SmtSolverPtr&);
    /** @} */

    /** Property: Database.
     *
     *  This is initialized by @ref configureExecution.
     *
     *  Thread safety: Not thread safe. */
    DatabasePtr database() const;

    /** Property: Test case to execute.
     *
     *  This property is initialized by @ref configureExecution.
     *
     *  Thread safety: Not thread safe. */
    TestCasePtr testCase() const;

    /** Property: Database ID for test case to execute.
     *
     *  This property is initialized by @ref configureExecution.
     *
     *  Thread safety: Not thread safe. */
    TestCaseId testCaseId() const;

    /** Property: Instruction partitioner.
     *
     *  This property is initialized by @ref configureExecution.
     *
     *  Thread safety: Not thread safe. */
    const Partitioner2::Partitioner& partitioner() const;

    /** Property: The concrete half of execution.
     *
     *  This property is initialized by @ref configureExecution.
     *
     *  Thread safety: Not thread safe. */
    ArchitecturePtr process() const;

    /** Property: The symbolic half of execution.
     *
     *  This property is initialized by @ref configureExecution.
     *
     *  Thread safety: Not thread safe. */
    Emulation::DispatcherPtr cpu() const;

    /** Describe command-line switches for settings.
     *
     *  Returns a list of command-line switches, organized into groups of related switches, that can be inserted into
     *  a command-line parser in order to parse all the concolic executor settings and generate documentation for them.
     *
     *  The supplied @ref settings reference provides the defaults for the documentation, and is also captured and used later
     *  as the destination for command-line switch arguments when the command-line is parsed and applied. */
    static std::vector<Sawyer::CommandLine::SwitchGroup> commandLineSwitches(Settings &settings /*in,out*/);

    /** Called before execution starts.
     *
     *  This can be called by the user, or is called automatically by @ref execute. Calling it separately allows the user
     *  to make some adjustments before execution starts, such as registering various callbacks. The @p architectureName
     *  must be a valid name for an architecture factory. */
    void configureExecution(const DatabasePtr&, const TestCasePtr&, const std::string &architectureName);

    /** Execute the test case.
     *
     *  Executes the test case to produce new test cases. If you've alreay called @ref configureExecution, then you don't
     *  need to pass the database, test case, or architecture name again (if you do, they better be the same as before).
     *
     * @{ */
    std::vector<TestCasePtr> execute();
    std::vector<TestCasePtr> execute(const DatabasePtr&, const TestCasePtr&, const std::string &architectureName);
    /** @} */

private:
    // Disassemble the specimen and cache the result in the database. If the specimen has previously been disassembled
    // then reconstitute the analysis results from the database.
    Partitioner2::Partitioner partition(const SpecimenPtr&, const std::string &architectureName);

    // Create the dispatcher, operators, and memory and register state for the symbolic execution.
    Emulation::DispatcherPtr makeDispatcher(const ArchitecturePtr&);

    // Create the underlying process and possibly fast forward it to the state at which it was when the test case was created.
    void startProcess();

    // Start up the symbolic part of the testing. This must happen after startProcess.
    void startDispatcher();

    // Run the execution
    void run();

    // Handle function calls. This is mainly for debugging so we have some idea where we are in the execution when an error
    // occurs.  Returns true if the call stack changed.
    bool updateCallStack(SgAsmInstruction*);

    // Print function call stack on multiple lines
    void printCallStack(std::ostream&);

    // Handle conditional branches
    void handleBranch(SgAsmInstruction*);

    // Generae a new test case. This must be called only after the SMT solver's assertions have been checked and found
    // to be satisfiable.
    void generateTestCase(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const SymbolicExpression::Ptr &childIp);

    // Save the specified symbolic state to the specified test case.
    void saveSymbolicState(const Emulation::RiscOperatorsPtr&, const TestCaseId&);

    // True if the two test cases are close enough that we only need to run one of them.
    bool areSimilar(const TestCasePtr&, const TestCasePtr&) const;

public:
    // TODO: Lots of properties to control the finer aspects of executing a test case!
};

} // namespace
} // namespace
} // namespace

#endif
#endif
