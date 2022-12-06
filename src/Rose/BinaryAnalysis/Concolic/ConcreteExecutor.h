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
    /** Shared ownership pointer. */
    using Ptr = ConcreteExecutorPtr;

private:
    std::string name_;
    DatabasePtr db_;

protected:
    // Allocating `instance` constructors should be implemented by the non-abstract subclasses.
    explicit ConcreteExecutor(const std::string&);      // for instantiating factories
    explicit ConcreteExecutor(const DatabasePtr&);
public:
    ~ConcreteExecutor();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Factories
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Register a concrete executor as a factory.
     *
     *  The specified executor is added to the end of a list of executor prototypical factory objects. When a new executor is
     *  needed, this list is scanned in reverse order until one of the @c matchFactory predicates for the prototypical object
     *  returns true, at which time a new copy of that object is created by passing the lookup arguments to its virtual @c
     *  instanceFromFactory constructor.
     *
     *  Thread safety: This method is thread safe. */
    static void registerFactory(const Ptr &factory);

    /** Remove a concrete executor factory from the registry.
     *
     *  The last occurrence of the specified factory is removed from the list of registered factories. This function returns
     *  true if a factory was removed, and false if no registered factories match.
     *
     *  Thread safety: This method is thread safe. */
    static bool deregisterFactory(const Ptr &factory);

    /** List of all registered factories.
     *
     *  The returned list contains the registered factories in the order they were registereed, which is the reverse order
     *  of how they're searched.
     *
     *  Thread safety: This method is thread safe. */
    static std::vector<Ptr> registeredFactories();

    /** Creates a suitable executor by name.
     *
     *  Scans the @ref registeredFactories list in the reverse order looking for a factory whose @ref matchFactory predicate
     *  (which accepts all but the first argument to this function) returns true. The first factory whose predicate returns
     *  true is used to create and return a new concrete executor object by invoking the factory's virtual @c
     *  instanceFromFactory constructor with the first argument of this function.
     *
     *  Thread safety: This method is thread safe. */
    static Ptr forge(const DatabasePtr&, const std::string &name);

    /** Predicate for matching a concrete executor factory by name. */
    virtual bool matchFactory(const std::string &name) const = 0;

    /** Virtual constructor for factories.
     *
     *  This creates a new object by calling the class method @c instance for the class of which @c this is a type. All
     *  arguments are passed to @c instance. */
    virtual Ptr instanceFromFactory(const DatabasePtr&) = 0;

    /** Returns true if this object is a factory.
     *
     *  Factories are created by the @c factory class methods rather than the usual @c instance class methods. A factory
     *  object should only be used to create other (non-factory) objects by registering it as a factory and eventually
     *  calling (directly or indirectly) its @ref instanceFromFactory object method. */
    bool isFactory() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Name.
     *
     *  The name of the executor originally comes from the executor factory, but can be changed on a per object
     *  basis.
     *
     * @{ */
    const std::string& name() const;
    void name(const std::string&);
    /** @} */

    /** Database provided to constructor. */
    DatabasePtr database() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Execution
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Execute one test case synchronously.
     *
     *  Returns the results from running the test concretely. Results are user-defined. The return value is never a null
     *  pointer. */
    virtual ConcreteResultPtr execute(const TestCasePtr&) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Old stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
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
