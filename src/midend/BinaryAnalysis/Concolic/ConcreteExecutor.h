#ifndef ROSE_BinaryAnalysis_Concolic_ConcreteExecutor_H
#define ROSE_BinaryAnalysis_Concolic_ConcreteExecutor_H
#include <Concolic/BasicTypes.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <boost/filesystem.hpp>
#include <rose_isnan.h>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Base class for user-defined concrete execution results.
 *
 *  Regardless of what data a subclass might add, all concrete execution results have a floating-point "rank" used to sort them
 *  when creating the list of test cases that should next run in the concolic (combined concrete and symbolic) executor. The
 *  convention is that those with lower ranks will run before those with higher ranks, although subclasses of @ref
 *  ExecutionManager can override this. The rank should be a real number (not NaN).
 *
 *  The subclasses must provide @c boost::serialization support which is used to store the user-defined results in the database
 *  and to reconstitute results objects from the database. Since this is a relatively expensive operation, the rank is also
 *  stored separately within the database. */
class ConcreteExecutorResult {
private:
    double rank_;
    bool isInteresting_;

public:
    ConcreteExecutorResult()
        : rank_(0.0), isInteresting_(true) {}

    explicit ConcreteExecutorResult(double rank)
        : rank_(rank), isInteresting_(true) {
        ASSERT_forbid(rose_isnan(rank));
    }

    virtual ~ConcreteExecutorResult() {}

    double rank() const { return rank_; }
    void rank(double r) { rank_ = r; }

    bool isInteresting() const { return isInteresting_; }
    void isInteresting(bool b) { isInteresting_ = b; }

private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(rank_);
    }
};

/** Base class for executing test cases concretely.
 *
 *  The user is expected to subclass this object in order to define the specifics of how to execute a test case concretely,
 *  measure some properties of the execution, and return those properties. A basic @ref LinuxExecutor subclass is already
 *  provided to give an example of how to run a program and measure its exit status. Other more complex executors might do
 *  things like measure code coverage.
 *
 *  ConcreteExecutor objects are expected to be used in single-threaded applications. Supporting multi-threaded concrete
 *  executors would be difficult since calling fork[2] from multi-threaded C++ programs is fraught with danger. Therefore, none
 *  of the methods in this API are thread-safe. */
class ConcreteExecutor: public Sawyer::SharedObject {
public:
    /** Reference counting pointer to a @ref ConcreteExecutor. */
    typedef Sawyer::SharedPointer<ConcreteExecutor> Ptr;

private:
    DatabasePtr db_;

protected:
    // Allocating constructors should be implemente by the non-abstract subclasses.
    explicit ConcreteExecutor(const DatabasePtr&);

public:
    ~ConcreteExecutor();

    /** Database provided to constructor. */
    DatabasePtr database() const;

    /** Execute one test case synchronously.
     *
     *  Returns the results from running the test concretely. Results are user-defined. The return value is never a null
     *  pointer. */
    virtual ConcreteExecutorResult* execute(const TestCasePtr&) = 0;

    // FIXME[Robb Matzke 2020-07-14]: This should be in a subclass
    /** \brief
     *  Sets an execution monitor for a test run. The execution monitor
     *  observes a test and computes a quality score that can be used to
     *  rank different executions (higher indicates better quality).
     *
     *  \details
     *  The execution monitor (e.g., execmon) needs to understand the
     *  following command line arguments:
     *    execmon -o outfile -- specimen test-arguments..
     *      -o outfile       a file containing two lines:
     *  or  --output=outfile (1) a human-readable integer value,
     *                           the exit code of the child process;
     *                       (2) a human-readable floating point value,
     *                           the quality score of the execution.
     *      --               separator between arguments to execmon
     *                       and test specification.
     *      specimen         the tested specimen
     *      test-arguments.. an arbitrary long argument list passed
     *                       to specimen.
     *
     *  @{
     */
    void executionMonitor(const boost::filesystem::path& executorName)
    {
      execmon = executorName;
    }

    // FIXME[Robb Matzke 2020-07-14]: This should be in a subclass
    boost::filesystem::path executionMonitor() const
    {
      return execmon;
    }
    /** @} */

private:
    // FIXME[Robb Matzke 2020-07-14]: This should be in a subclass
    boost::filesystem::path execmon; // the execution monitor
};

} // namespace
} // namespace
} // namespace

#endif
#endif
