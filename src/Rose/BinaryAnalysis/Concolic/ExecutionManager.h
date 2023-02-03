#ifndef ROSE_BinaryAnalysis_Concolic_ExecutionManager_H
#define ROSE_BinaryAnalysis_Concolic_ExecutionManager_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/Settings.h>

#include <Rose/Yaml.h>

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
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Reference counting pointer to an @ref ExecutionManager. */
    using Ptr = Sawyer::SharedPointer<ExecutionManager>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    DatabasePtr database_;
    ConcolicExecutorSettings concolicExecutorSettings_;
    const Yaml::Node config_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Construction and destruction
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual ~ExecutionManager();
protected:
    explicit ExecutionManager(const DatabasePtr&);
    ExecutionManager(const DatabasePtr&, const ConcolicExecutorSettings&, const Yaml::Node&);

public:
    /** Configure a manager from a YAML file.
     *
     *  This reads and parses the specified YAML file and creates a new manager based on the configuration in that file.  The
     *  file should have a top-level map with the following keys:
     *
     *  @li "database" is the URL for the database that will be opened. The database must exist and be initialized already.
     *
     *  @li "test-suite" is the optional name of the test suite for the test campaign being run. If it is missing then the most
     *  recently created test suite is used.
     *
     *  @li "architecture" describes what architecture is being tested, and is one of the names of the factories returned by
     *  the @ref Concolic::Architecture::registeredFactories method.
     *
     *  @li "concrete" is the base name of the concrete executor. The full name is formed by concatenating the architecture
     *  name, two colons ("::") and the concrete name and must result in one of the names of the concrete executors returned by
     *  the @ref Concolic::ConcreteExecutor::registeredFactories method.
     *
     *  @li "shared-memory" is a list of YAML maps describing each shared memory region and its behavior. At a minimum, each
     *  shared memory YAML map must contain a "driver" that names the class of object handling the shared memory, an "address"
     *  to desribe the lowest address in this shared memory region, and the "size" of the region measured in bytes. The YAML
     *  map is passed to the initializer for the object that handles the memory.
     *
     *  @li "concolic-stride" is an optional positive integer that indicates the maximum number of test cases that are run
     *  concolically at a time, before running any pending concrete tests. The default is one.
     *
     *  A reference to the YAML configuration tree is stored in the returned object. The existing database is opened and will
     *  be closed by the destructor. A particular test suite within the database is active. */
    static Ptr instance(const ConcolicExecutorSettings&, const Yaml::Node&);

    /** Allocating constructor to open an existing database.
     *
     *  The specified database, which must exist and be initialized, is opened. The database will be closed by the destructor.
     *
     *  If the database contains more than one test suite, then the latest created test suite is opened. See also, @ref
     *  testSuites, @ref testSuite.
     *
     *  Throws an @ref Exception if the database cannot be opened. */
    static Ptr open(const std::string &databaseUrl);

    /** Allocating constructor to create a new database.
     *
     *  The specified database is initialized with a specimen and an initial test case. For SQLite databases, the database file
     *  is created, removing any previous file with the same name. For PostgreSQL databases, the database is assumed to exit in
     *  an empty state and all necessary tables are created. The database will be closed by the destructor.
     *
     *  If a non-empty test suite name is specified then it will be used as the name for the test suite. Otherwise a test suite
     *  will be created whose name is based on the executable name.
     *
     *  An @ref Exception is thrown for any errors. */
    static Ptr create(const std::string &databaseUrl, const boost::filesystem::path &executableName,
                      const std::vector<std::string> &arguments, const std::string &testSuiteName = "");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Concolic executor settings.
     *
     * @{ */
    const ConcolicExecutorSettings& concolicExecutorSettings() const;
    ConcolicExecutorSettings& concolicExecutorSettings();
    void concolicExecutorSettings(const ConcolicExecutorSettings&);
    /** @} */

    /** Property: Database.
     *
     *  The database used by this manager.  The database is set in the constructor and cannot be changed later. */
    DatabasePtr database() const;

    /** Property: List of available test suites.
     *
     *  Every database has one or more test suites. A test suite controls which specimens and test cases compose a testing
     *  campaign. Users can organize databases so each database contains a single test suite, or a single database contains
     *  many test suites. The former method is often used with SQLite databases (which are just files) while the latter is
     *  sometimes used with PostgreSQL databases. */
    std::vector<TestSuiteId> testSuites() const;

    /** Property: Current test suite.
     *
     *  The test suite being used for the current test campaign.
     *
     * @{ */
    TestSuitePtr testSuite() const;
    void testSuite(const TestSuitePtr&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Actions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
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
    virtual void insertConcreteResults(const TestCasePtr&, const ConcreteResultPtr&);

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
    virtual void run();
};

} // namespace
} // namespace
} // namespace

#endif
#endif
