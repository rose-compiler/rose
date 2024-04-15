#ifndef ROSE_BinaryAnalysis_Concolic_ConcolicExecutor_H
#define ROSE_BinaryAnalysis_Concolic_ConcolicExecutor_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/Settings.h>

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/Yaml.h>

#include <Sawyer/CommandLine.h>
#include <Sawyer/FileSystem.h>

#include <ostream>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

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
    ConcolicExecutorSettings settings_;
    std::vector<FunctionCall> functionCallStack_;
    Sawyer::FileSystem::TemporaryDirectory tmpDir_;

    // These can be configured by the user before calling configureExecution.
    SmtSolverPtr solver_;                               // solver used during execution

    // These are initialized by configureExecution
    TestCasePtr testCase_;                              // currently executing test case
    TestCaseId testCaseId_;                             // database ID for currently executing test case
    DatabasePtr db_;                                    // database for all this stuff
    Partitioner2::PartitionerPtr partitioner_;          // used during execution
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
    const ConcolicExecutorSettings& settings() const;
    ConcolicExecutorSettings& settings();
    void settings(const ConcolicExecutorSettings&);
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
    Partitioner2::PartitionerConstPtr partitioner() const;

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
    static std::vector<Sawyer::CommandLine::SwitchGroup> commandLineSwitches(ConcolicExecutorSettings &settings /*in,out*/);

    /** Called before execution starts.
     *
     *  This can be called by the user, or is called automatically by @ref execute. Calling it separately allows the user
     *  to make some adjustments before execution starts, such as registering various callbacks.
     *
     *  The configuration is a YAML document. The top-level node must be a map with the keys described in the documentation
     *  for @ref Concolic::ExecutionManager::instance. */
    void configureExecution(const DatabasePtr&, const TestCasePtr&, const Yaml::Node &config);

    /** Execute the test case.
     *
     *  Executes the test case to produce new test cases. If you've alreay called @ref configureExecution, then you don't
     *  need to pass the database, test case, or architecture name again (if you do, they better be the same as before).
     *
     * @{ */
    std::vector<TestCasePtr> execute();
    std::vector<TestCasePtr> execute(const DatabasePtr&, const TestCasePtr&, const Yaml::Node &config);
    /** @} */

private:
    // Disassemble the specimen and cache the result in the database. If the specimen has previously been disassembled
    // then reconstitute the analysis results from the database.
    Partitioner2::PartitionerPtr partition(const SpecimenPtr&, const ArchitecturePtr&);

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
