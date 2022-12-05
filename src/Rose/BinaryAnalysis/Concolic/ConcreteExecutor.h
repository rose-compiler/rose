#ifndef ROSE_BinaryAnalysis_Concolic_ConcreteExecutor_H
#define ROSE_BinaryAnalysis_Concolic_ConcreteExecutor_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <boost/filesystem.hpp>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Base class for executing test cases concretely.
 *
 *  The user is expected to subclass this object in order to define the specifics of how to execute a test case concretely,
 *  measure some properties of the execution, and return those properties. A basic @ref LinuxConcrete subclass is already
 *  provided, when ROSE is build on Linux, to give an example of how to run a program and measure its exit status. Other more
 *  complex executors might do things like measure code coverage.
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
    virtual ConcreteExecutorResultPtr execute(const TestCasePtr&) = 0;

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
