#ifndef ROSE_BinaryAnalysis_Concolic_ExecutionManager_H
#define ROSE_BinaryAnalysis_Concolic_ExecutionManager_H
#include <Concolic/BasicTypes.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <boost/noncopyable.hpp>
#include <Sawyer/SharedObject.h>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Base class for managing an entire concolic testing run.
 *
 *  An @ref ExecutionManager is responsible for running an entire test suite either starting from scratch or restarting from a
 *  previous state that was saved in a database. */
class ExecutionManager: boost::noncopyable, public Sawyer::SharedObject {
public:
    /** Reference counting pointer to an @ref ExecutionManager. */
    typedef Sawyer::SharedPointer<ExecutionManager> Ptr;

private:
    DatabasePtr database_;

protected:
    // Subclasses should implement allocating constructors
    explicit ExecutionManager(const DatabasePtr &db);

public:
    virtual ~ExecutionManager();

    /** Property: Database.
     *
     *  The database used by this manager.  The database is set in the constructor and cannot be changed later. */
    DatabasePtr database() const;

    /** Next test case for concrete execution.
     *
     *  Returns up to @p n (default unlimited) test cases that need to be run concretely. A test case needs to be run
     *  concretely if it has no results from a previous concrete run.
     *
     * @{ */
    virtual std::vector<TestCaseId> pendingConcreteResults(size_t n = UNLIMITED);
    TestCaseId pendingConcreteResult() /*final*/;
    /** @} */

    /** Insert results of a concrete run.
     *
     *  Inserts into the database the results of a concrete run of a test case. Concrete test results always have two parts: a
     *  floating point number for ranking relative to other concrete results, and the result details.  The detailed results are
     *  user defined and stored in the database in XML format, while the rank is duplicated in a floating point field. */
    virtual void insertConcreteResults(const TestCasePtr&, const ConcreteExecutorResult &details);

    /** Next test case for concolic execution.
     *
     *  Returns up to @p n (default unlimited) test cases that need to be run concolically. A test case needs to be run
     *  concolically if it is not marked as having completed the concolic run.
     *
     * @{ */
    virtual std::vector<TestCaseId> pendingConcolicResults(size_t n = UNLIMITED);
    TestCaseId pendingConcolicResult() /*final*/;
    /** @} */

    /** Insert results from a concolic execution.
     *
     *  When a concolic execution completes it will have produced some number of new test cases that should be inserted into
     *  the database. The subclass is reponsible for pruning the list of produced test cases by removing those that have been
     *  tested already or which wouldn't contribute any new results. */
    virtual void insertConcolicResults(const TestCasePtr &original, const std::vector<TestCasePtr> &newCases);

    /** Predicate to determine whether testing is completed.
     *
     *  Testing is done when there are no more test cases that need concrete or concolic results. */
    virtual bool isFinished() const;

    /** Start running.
     *
     *  Runs concrete and concolic executors until the application is interrupted or there's nothing left to do. Subclasses
     *  will likely reimplement this method in order to do parallel processing, limit execution time, etc. */
    virtual void run() = 0;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
