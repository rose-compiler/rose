#ifndef ROSE_BinaryAnalysis_Concolic_ConcolicExecutor_H
#define ROSE_BinaryAnalysis_Concolic_ConcolicExecutor_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/Concolic/LinuxI386Executor.h>
#include <Rose/BinaryAnalysis/Debugger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/DispatcherX86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/SymbolicSemantics.h>
#include <Sawyer/FileSystem.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables that exist in the specimen.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Describes where a symbolic variable came from.
 *
 *  Every variable corresponds to an ExecutionEvent so that if a variable's value is changed then the corresponding execution
 *  event (when copied to the newly generated test case) is also updated. For instance, if main's argc changes from 2 to 1 then
 *  the corresponding memory-write event for the new test case will be set to 2 instead of 1 when it's copied from the parent
 *  test case. Thus, when the new test case runs and the event is reached, the event's action will cause the concrete
 *  execution's argc to be overwritten with 2. */
class InputVariables {
public:
    class Variable {
    public:
        /** From whence a variable came. */
        enum Whence {
            INVALID,                                    /**< Provenance record is invalid (default constructed). */
            PROGRAM_ARGUMENT_COUNT,                     /**< Number of program arguments. */
            PROGRAM_ARGUMENT,                           /**< Variable is (part of) a program argument. */
            ENVIRONMENT,                                /**< Variable is (part of) a program environment. */
            SYSTEM_CALL_RETVAL                          /**< Variable is return value of system call. */
        };

    private:
        Whence whence_;
        ExecutionEventId executionEventId_;             // Event that adjusts the value in concrete execution
        union {
            struct {
                size_t idx1, idx2;
            } arrayOfStrings;
            struct {
                size_t serialNumber;
            } systemCall;
        };

    public:
        Variable()
            : whence_(INVALID) {}

        /** Create a variable for the program argument count. */
        static Variable programArgc(const ExecutionEventId &eeid) {
            Variable v;
            v.whence_ = PROGRAM_ARGUMENT_COUNT;
            v.executionEventId_ = eeid;
            return v;
        }

        /** Create a variable for a program argument. */
        static Variable programArgument(const ExecutionEventId &eeid, size_t argIdx, size_t charIdx) {
            Variable v;
            v.whence_ = PROGRAM_ARGUMENT;
            v.executionEventId_ = eeid;
            v.arrayOfStrings.idx1 = argIdx;
            v.arrayOfStrings.idx2 = charIdx;
            return v;
        }

        /** Create a variable for a program environment variable. */
        static Variable environmentVariable(const ExecutionEventId &eeid, size_t envIdx, size_t charIdx) {
            Variable v;
            v.whence_ = ENVIRONMENT;
            v.executionEventId_ = eeid;
            v.arrayOfStrings.idx1 = envIdx;
            v.arrayOfStrings.idx2 = charIdx;
            return v;
        }

        /** Create a variable for a system call return value. */
        static Variable systemCallReturn(const ExecutionEventId &eeid) {
            Variable v;
            v.whence_ = SYSTEM_CALL_RETVAL;
            v.executionEventId_ = eeid;
            return v;
        }

        /** Associated execution event. */
        ExecutionEventId executionEventId() const {
            return executionEventId_;
        }

        /** Index of string in array. */
        size_t variableIndex() const {
            switch (whence_) {
                case PROGRAM_ARGUMENT:
                case ENVIRONMENT:
                    return arrayOfStrings.idx1;
                default:
                    ASSERT_not_reachable("variable index not available");
            }
        }

        /** Index of character within string. */
        size_t charIndex() const {
            switch (whence_) {
                case PROGRAM_ARGUMENT:
                case ENVIRONMENT:
                    return arrayOfStrings.idx2;
                default:
                    ASSERT_not_reachable("character index not available");
            }
        }

        /** Serial number fo system call. */
        size_t serialNumber() {
            switch (whence_) {
                case SYSTEM_CALL_RETVAL:
                    return systemCall.serialNumber;
                default:
                    ASSERT_not_reachable("serial number not available");
            }
        }
        
        /** What kind of variable this is. */
        Whence whence() const { return whence_; }

        void print(std::ostream&) const;                /**< Print the variable name. */

        friend std::ostream& operator<<(std::ostream &out, const Variable &x) {
            x.print(out);
            return out;
        }
    };

private:
    typedef Sawyer::Container::Map<uint64_t, Variable> Variables; // map symbolic variable ID to input Variable
    Variables variables_;

public:
    /** Insert a record describing the number of program arguments. */
    void insertProgramArgumentCount(const ExecutionEventId&, const SymbolicExpr::Ptr&);

    /** Insert a record for a program argument.
     *
     *  The @p i and @p j are the indexes for the <code>char *argv[]</code> argument of a C or C++ program's "main" function. */
    void insertProgramArgument(const ExecutionEventId&, size_t i, size_t j, const SymbolicExpr::Ptr&);

    /** Insert a record for an environment variable.
     *
     *  The @p i and @p j are the indexes for the <code>char *envp[]</code> argument of a C or C++ program's "main" function. */
    void insertEnvironmentVariable(const ExecutionEventId&, size_t i, size_t j, const SymbolicExpr::Ptr&);

    /** Insert a record for a system call return. */
    void insertSystemCallReturn(const ExecutionEventId&, const SymbolicExpr::Ptr&);

    /** Find a variable record when given a symbolic variable name. */
    Variable get(const std::string &symbolicVarName) const;

    /** Print all defined variables. */
    void print(std::ostream&, const std::string &prefix = "") const;

    friend std::ostream& operator<<(std::ostream &out, const InputVariables &x) {
        x.print(out);
        return out;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Concolic emulation semantics.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Semantics for concolic execution. */
namespace Emulation {

typedef InstructionSemantics2::SymbolicSemantics::Formatter SValueFormatter; /**< How to format expressions when printing. */
typedef InstructionSemantics2::SymbolicSemantics::SValuePtr SValuePtr; /**< Pointer to semantic values. */
typedef InstructionSemantics2::SymbolicSemantics::SValue SValue; /**< Type of semantic values. */
typedef InstructionSemantics2::SymbolicSemantics::RegisterStatePtr RegisterStatePtr; /**< Pointer to semantic registers. */
typedef InstructionSemantics2::SymbolicSemantics::RegisterState RegisterState; /**< Type of semantic register space. */
typedef InstructionSemantics2::SymbolicSemantics::MemoryStatePtr MemoryStatePtr; /**< Pointer to semantic memory. */
typedef InstructionSemantics2::SymbolicSemantics::MemoryState MemoryState; /**< Type of semantic memory space. */
typedef InstructionSemantics2::SymbolicSemantics::StatePtr StatePtr; /**< Pointer to semantic machine state. */
typedef InstructionSemantics2::SymbolicSemantics::State State; /**< Semantic machine state. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr; /**< Pointer to semantic operations. */

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

/** Description of a system call. */
struct SystemCall {
    rose_addr_t ip;                                     // where the system call occurs
    uint64_t functionNumber;                            // which system call
    std::vector<uint64_t> arguments;                    // the concrete arguments for the system call
};

/** Semantic operations. */
class RiscOperators: public InstructionSemantics2::SymbolicSemantics::RiscOperators {
public:
    /** Base class. */
    typedef InstructionSemantics2::SymbolicSemantics::RiscOperators Super;

    /** Special "path" Boolean register. */
    const RegisterDescriptor REG_PATH;

    /** Settings for the emulation. */
    struct Settings {
        bool markingArgvAsInput;                        /** Whether to mark the characters of the argv strings as inputs. */
        bool markingEnvpAsInput;                        /** Whether to mark the characters of the envp strings as inputs. */

        Settings()
            : markingArgvAsInput(true),                 // normally considered as input
              markingEnvpAsInput(false)                 // not input for now since the DB doesn't store them
            {}
    };

private:
    Settings settings_;                                 // emulation settings
    DatabasePtr db_;                                    // concolic database connection
    TestCasePtr testCase_;                              // test case whose instructions are being processed
    const Partitioner2::Partitioner &partitioner_;      // ROSE disassembly info about the specimen
    LinuxI386Executor::Ptr process_;                    // subordinate process
    InputVariables &inputVariables_;                    // where did symbolic variables come from?
    Sawyer::Optional<SystemCall> systemCall_;           // set each time a system call is encountered

protected:
    /** Allocating constructor. */
    RiscOperators(const Settings &settings, const DatabasePtr &db, const TestCasePtr &testCase,
                  const Partitioner2::Partitioner &partitioner, const LinuxI386ExecutorPtr &process,
                  InputVariables &inputVariables, const InstructionSemantics2::BaseSemantics::StatePtr &state,
                  const SmtSolverPtr &solver)
        : Super(state, solver), REG_PATH(state->registerState()->registerDictionary()->findOrThrow("path")),
          settings_(settings), db_(db), testCase_(testCase), partitioner_(partitioner), process_(process),
          inputVariables_(inputVariables) {
        ASSERT_not_null(db);
        ASSERT_not_null(testCase);
        ASSERT_not_null(process);
        ASSERT_not_null(state);
        ASSERT_not_null(solver);
        name("Concolic-symbolic");
        (void) SValue::promote(state->protoval());
    }

public:
    /** Allocating constructor. */
    static RiscOperatorsPtr instance(const Settings &settings, const DatabasePtr&, const TestCasePtr&,
                                     const Partitioner2::Partitioner&, const LinuxI386ExecutorPtr &process,
                                     InputVariables&, const InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
                                     const SmtSolverPtr &solver = SmtSolverPtr());

    /** Dynamic pointer downcast. */
    static RiscOperatorsPtr promote(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&);

    // Overrides documented in base class
    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_implemented("[Robb Matzke 2019-09-24]");
    }
    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::StatePtr &state,
           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_implemented("[Robb Matzke 2019-09-24]");
    }

public:
    /** Property: Settings.
     *
     *  The settings are read-only, set when this object was created. */
    const Settings& settings() const {
        return settings_;
    }

    /** Property: Partitioner. */
    const Partitioner2::Partitioner& partitioner() const {
        return partitioner_;
    }

    /** Property: Test case. */
    TestCasePtr testCase() const;

    /** Property: Database. */
    DatabasePtr database() const;

    /** Property: Concrete half of the concolic executor semantics. */
    LinuxI386Executor::Ptr process() const {
        return process_;
    }

    /** Number of bits in a word.
     *
     *  The definition of "word" is the natural width of the instruction pointer, stack pointer, most general-purpose
     *  registers, etc. */
    size_t wordSizeBits() const;

    /** Register definitions. */
    const RegisterDictionary* registerDictionary() const;

    /** Create info about program arguments.
     *
     *  This function creates the execution events and symbolic variables for program arguments (argc, argv, envp) and creates
     *  a mapping from the symbolic variable to its corresponding concrete execution event. */
    void createProgramArguments(const SmtSolver::Ptr&);

    /** Print input variables.
     *
     *  Shows the mapping from input variables to their symbolic values. */
    void printInputVariables(std::ostream&) const;

    /** Get system call information from machine state.
     *
     *  @{ */
    uint64_t systemCallFunctionNumber();
    InstructionSemantics2::BaseSemantics::SValuePtr systemCallArgument(size_t idx);
    RegisterDescriptor systemCallReturnRegister();
    InstructionSemantics2::BaseSemantics::SValuePtr systemCallReturnValue();
    InstructionSemantics2::BaseSemantics::SValuePtr systemCallReturnValue(const InstructionSemantics2::BaseSemantics::SValuePtr&);
    /** @} */

    /** Information about a system call.
     *
     *  This information is updated each time a system call is encountered.
     *
     * @{ */
    const Sawyer::Optional<SystemCall>& systemCall() const {
        return systemCall_;
    }
    Sawyer::Optional<SystemCall>& systemCall() {
        return systemCall_;
    }
    /** @} */

    /** Actions to perform after a system call occurs.
     *
     *  If the previous call to Dispatcher::processInstruction wasn't a system call then this is a no-op. */
    void finishSystemCall();

public:
    // Base class overrides -- the acutal RISC operations
    virtual void interrupt(int majr, int minr) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readRegister(RegisterDescriptor reg, const InstructionSemantics2::BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readRegister(RegisterDescriptor reg) ROSE_OVERRIDE {
        return readRegister(reg, undefined_(reg.nBits()));
    }

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    peekRegister(RegisterDescriptor reg, const InstructionSemantics2::BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readMemory(RegisterDescriptor segreg, const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
               const InstructionSemantics2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    peekMemory(RegisterDescriptor segreg, const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics2::BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;

private:
    // Handles a Linux system call of the INT 0x80 variety. This is called before the concrete system call executes. See also,
    // finishSystemCall.
    void startSystemCall();

    // Special handling for some system calls
    void doSyscallExit(uint64_t);
};

/**< Pointer to virtual CPU. */
typedef boost::shared_ptr<class Dispatcher> DispatcherPtr;

/** CPU for concolic emulation. */
class Dispatcher: public InstructionSemantics2::DispatcherX86 {
    typedef InstructionSemantics2::DispatcherX86 Super;
protected:
    /** Constructor. */
    explicit Dispatcher(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops)
        : Super(ops, unwrapEmulationOperators(ops)->wordSizeBits(), unwrapEmulationOperators(ops)->registerDictionary()) {}

public:
    /** Allocating constructor. */
    static DispatcherPtr instance(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops) {
        return DispatcherPtr(new Dispatcher(ops));
    }

public:
    /** Concrete instruction pointer. */
    rose_addr_t concreteInstructionPointer() const;

    /** Single step the concrete part of the executor with absolutely no regard for keeping the symbolic part up to date. */
    void concreteSingleStep();

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
    static RiscOperatorsPtr unwrapEmulationOperators(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&);

    /** The concrete half of processInstruction. */
    void processConcreteInstruction(SgAsmInstruction*);

public:
    // overrides
    virtual void processInstruction(SgAsmInstruction*) ROSE_OVERRIDE;
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
    InputVariables inputVariables_;
    std::vector<FunctionCall> functionCallStack_;

protected:
    ConcolicExecutor() {}

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
    const Settings& settings() const { return settings_; }
    Settings& settings() { return settings_; }
    /** @} */

    /** Describe command-line switches for settings.
     *
     *  Returns a list of command-line switches, organized into groups of related switches, that can be inserted into
     *  a command-line parser in order to parse all the concolic executor settings and generate documentation for them.
     *
     *  The supplied @ref settings reference provides the defaults for the documentation, and is also captured and used later
     *  as the destination for command-line switch arguments when the command-line is parsed and applied. */
    static std::vector<Sawyer::CommandLine::SwitchGroup> commandLineSwitches(Settings &settings /*in,out*/);

    /** Execute the test case.
     *
     *  Executes the test case to produce new test cases. */
    std::vector<TestCasePtr> execute(const DatabasePtr&, const TestCasePtr&);

private:
    // Disassemble the specimen and cache the result in the database. If the specimen has previously been disassembled
    // then reconstitute the analysis results from the database.
    Partitioner2::Partitioner partition(const DatabasePtr&, const SpecimenPtr&);

    // Create the process for the concrete execution.
    LinuxI386ExecutorPtr makeProcess(const DatabasePtr&, const TestCasePtr&, const boost::filesystem::path &tempDir);

    // Run the execution
    void run(const DatabasePtr&, const TestCasePtr&, const Emulation::DispatcherPtr&);

    // Handle function calls. This is mainly for debugging so we have some idea where we are in the execution when an error
    // occurs.  Returns true if the call stack changed.
    bool updateCallStack(const Emulation::DispatcherPtr&, SgAsmInstruction*);

    // Print function call stack on multiple lines
    void printCallStack(std::ostream&);

    // Handle conditional branches
    void handleBranch(const DatabasePtr&, const TestCasePtr&, const Emulation::DispatcherPtr&, SgAsmInstruction*,
                      const SmtSolverPtr&);

    // Generae a new test case. This must be called only after the SMT solver's assertions have been checked and found
    // to be satisfiable.
    void generateTestCase(const DatabasePtr&, const TestCasePtr&, const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
                          const SmtSolverPtr&);

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
