#ifndef ROSE_BinaryAnalysis_Concolic_Database_H
#define ROSE_BinaryAnalysis_Concolic_Database_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/Constants.h>
#include <rose_strtoull.h>

#include <Sawyer/BiMap.h>
#include <Sawyer/Database.h>
#include <Sawyer/Optional.h>

#include <boost/numeric/conversion/cast.hpp>

#include <ctype.h>
#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Database.
 *
 *  A database holds the entire state for a concolic testing system consisting of one or more test suites. A @ref Database
 *  object is always connected to exactly one database and limits the scope of its operations to exactly one test suite (except
 *  where noted).
 *
 *  A @ref Database object refers to persistent storage and supports both PostgreSQL databases and SQLite3 databases and the
 *  the possibility of adding other RDMSs later.
 *
 *  Objects within a database have an ID number, and these ID numbers are type-specific. When an object is inserted (copied)
 *  into a database a new ID number is returned. The @ref Database object memoizes the association between object IDs and
 *  objects. */
class Database: public Sawyer::SharedObject, public Sawyer::SharedFromThis<Database>, boost::noncopyable {
public:
    /** Reference counting pointer to @ref Database. */
    typedef Sawyer::SharedPointer<Database> Ptr;
    
private:
    Sawyer::Database::Connection connection_;

    // Memoization of ID to object mappings
    Sawyer::Container::BiMap<SpecimenId, SpecimenPtr> specimens_;
    Sawyer::Container::BiMap<TestCaseId, TestCasePtr> testCases_;
    Sawyer::Container::BiMap<TestSuiteId, TestSuitePtr> testSuites_;
    Sawyer::Container::BiMap<ExecutionEventId, ExecutionEventPtr> executionEvents_;

    TestSuiteId testSuiteId_;                           // database scope is restricted to this single test suite

protected:
    Database();

public:
    ~Database();

    /** Open an existing database.
     *
     *  The database's current test suite is set to the latest created test suite. It can be reset to no test suite if desired by setting
     *  the @ref testSuite property to nothing.
     *
     *  Throws an @ref Exception if the database does not exist. */
    static Ptr instance(const std::string &url);

    /** Low-level database connection. */
    Sawyer::Database::Connection connection() {
        return connection_;
    }

    /** Create a new database and test suite.
     *
     *  For database management systems that support it, a new database is created, possibly overwriting any previous data if
     *  the database already existed. SQLite3 databases can be created this way because they're just files in the local
     *  filesystem, but PostgreSQL databases need to be created through the DBMS. Throws an @ref Exception if the new database
     *  could not be created.
     *
     *  Once the database is created, a new test suite with the given name is created.
     * @{
     */
    static Ptr create(const std::string &url);
    static Ptr create(const std::string &url, const std::string &testSuiteName);
    /** @} */

    //------------------------------------------------------------------------------------------------------------------------
    // Test suites
    //------------------------------------------------------------------------------------------------------------------------

    /** All test suites.
     *
     *  Returns information about all the test suites that exist in this database. This is essentially the table of contents
     *  for the database. */
    std::vector<TestSuiteId> testSuites();

    /** Property: the current test suite.
     *
     *  If the database has a current test suite, then queries are limited in scope to return results associated with that test
     *  suite.  Setting the test suite property also updates the database with information about the test suite, creating the
     *  test suite if necessary.
     *
     * @{ */
    TestSuitePtr testSuite();
    TestSuiteId testSuite(const TestSuitePtr&);
    /** @} */

    //------------------------------------------------------------------------------------------------------------------------
    // Specimens
    //------------------------------------------------------------------------------------------------------------------------

    /** All specimens.
     *
     *  If this database object has a current test suite, then the return value is limited to specimens used by that test
     *  suite, otherwise all specimens are returned. */
    std::vector<SpecimenId> specimens();

    /** Specimens that are part of the specified test suite. */
    std::vector<SpecimenId> specimens(TestSuiteId);

    /** Erase all specimens for the specified test suite. */
    void eraseSpecimens(TestSuiteId);

    //------------------------------------------------------------------------------------------------------------------------
    // Test cases
    //------------------------------------------------------------------------------------------------------------------------

    /** All test cases.
     *
     *  If this database object has a current test suite, then the return value is limited to test cases used by that test
     *  suite, otherwise all test cases are returned. */
    std::vector<TestCaseId> testCases();

    /** Test cases for a specific specimen. */
    std::vector<TestCaseId> testCases(SpecimenId);

    /** Erase all test cases for a specimen. */
    void eraseTestCases(SpecimenId);

    //------------------------------------------------------------------------------------------------------------------------
    // Execution events
    //------------------------------------------------------------------------------------------------------------------------

    /** All execution events.
     *
     *  If this database object has a current test suite, then the return value is limited to execution events used by that
     *  test suite, otherwise all execution events are returned. */
    std::vector<ExecutionEventId> executionEvents();

    /** All execution events for a particular test case.
     *
     *  The events are sorted according to @ref ExecutionLocation::isSorted. */
    std::vector<ExecutionEventId> executionEvents(TestCaseId);

    /** Number of execution events for a particular test case. */
    size_t nExecutionEvents(TestCaseId);

    /** Execution events at a specific location.
     *
     *  Returns the execution events for a specific location. All events for the primary key of the location are returned,
     *  sorted according to @ref ExecutionLocation::isSorted. */
    std::vector<ExecutionEventId> executionEvents(TestCaseId, uint64_t primaryKey);

    /** Execution events from the specified event onward.
     *
     *  The returned events are sorted according to @ref ExecutionLocation::isSorted. */
    std::vector<ExecutionEventId> executionEventsSince(TestCaseId, ExecutionEventId startingAt);

    /** Primary keys for the location events.
     *
     *  The return value is the vector for all the primary location key values for the specified test case. */
    std::vector<uint64_t> executionEventKeyFrames(TestCaseId);

    /** Erase all events for a test case. */
    void eraseExecutionEvents(TestCaseId);

    //------------------------------------------------------------------------------------------------------------------------
    // Overloaded methods for all objects.
    //------------------------------------------------------------------------------------------------------------------------

    /** Reconstitute a object from a database ID.
     *
     *  The association between object and ID is memoized. If @p update is yes and a memoized object is being returned, then
     *  also updates the object with the current values from the database. If the ID is invalid then an exception is thrown.
     *
     * @{ */
    TestSuitePtr object(TestSuiteId, Update update = Update::YES);
    TestCasePtr object(TestCaseId, Update update = Update::YES);
    SpecimenPtr object(SpecimenId, Update update = Update::YES);
    ExecutionEventPtr object(ExecutionEventId, Update update = Update::YES);
    /** @} */

    /** Returns an ID number for an object, optionally writing to the database.
     *
     *  If the object exists in the database (i.e., returned as the result of some query) then its ID is returned and the
     *  database is optionally updated. On the other hand, if the object does not exist in the database then it will be created
     *  only if @p update is yes and its new ID is returned. If the object doesn't exist and isn't created then an invalid ID
     *  is returned.
     *
     * @{ */
    TestSuiteId id(const TestSuitePtr&, Update update = Update::YES);
    TestCaseId id(const TestCasePtr&, Update update = Update::YES);
    SpecimenId id(const SpecimenPtr&, Update update = Update::YES);
    ExecutionEventId id(const ExecutionEventPtr&, Update update = Update::YES);
    /** @} */

    /** Deletes an object from the database.
     *
     * @{ */
    TestSuiteId erase(TestSuiteId);
    TestCaseId erase(TestCaseId);
    SpecimenId erase(SpecimenId);
    ExecutionEventId erase(ExecutionEventId);
    /** @} */

    /** Saves an object.
     *
     *  This is a more self-documenting name for calling @ref id for the sole purpose of saving (creating or updating) an
     *  object's database representation. */
    template<class ObjectPointer>
    typename ObjectTraits<typename ObjectPointer::Pointee>::Id
    save(const ObjectPointer &obj) {
        return id(obj);
    }

    /** Save multiple objects.
     *
     *  This is just a shortcut for calling @ref save in a loop. */
    template<class ObjectPointer>
    void save(const std::vector<ObjectPointer> &objects) {
        for (auto object: objects)
            save(object);
    }

    /** Obtains multiple objects from multiple IDs.
     *
     *  This is just a shortcut for calling @ref object in a loop. */
    template<class Id>
    std::vector<typename Id::Pointer>
    objects(const std::vector<Id> &ids, Update update = Update::YES) {
        std::vector<typename Id::Pointer> retval;
        for (auto id: ids)
            retval.push_back(object(id, update));
        return retval;
    }

    /** Obtains multiple IDs from multiple objects.
     *
     *  This is just a shortcut for calling @ref id in a loop. */
    template<class ObjectPointer>
    std::vector<typename ObjectTraits<ObjectPointer>::Id>
    ids(const std::vector<ObjectPointer> &objects, Update update = Update::YES) {
        std::vector<typename ObjectTraits<ObjectPointer>::Id> retval;
        for (auto object: objects)
            retval.push_back(id(object, update));
        return retval;
    }

    /** Finds a test suite by name or ID.
     *
     *  Returns the (unique) @ref TestSuite object has the specified name. If no such test suite exists and the specified name
     *  can be parsed as an object ID (see constructors for @ref ObjectId) returns the test suite with the specified ID. If no
     *  matches are found by either mechanism then a null pointer is returned. This method is intended to be used mainly to
     *  convert command-line arguments to test suites. */
    TestSuitePtr findTestSuite(const std::string &nameOrId);

    /** Finds all specimens having the specified name.
     *
     *  If the database is restricted to a test suite (see @ref testSuite) then the returned specimens are only those that
     *  are part of the current test suite and have the specified name. Specimen names need not be unique or non-empty. */
    std::vector<SpecimenId> findSpecimensByName(const std::string &name);

    //------------------------------------------------------------------------------------------------------------------------
    // Cached info about disassembly. This is large data. Each specimen has zero or one associated RBA data blob.
    //------------------------------------------------------------------------------------------------------------------------

    /** Check whether a specimen has associated RBA data.
     *
     *  Returns true if the indicated specimen has associated ROSE Binary Analysis (RBA) data, and false if it doesn't. Each
     *  specimen can have zero or one associated RBA data blob. The specimen ID must be valid.
     *
     *  Thread safety: Not thread safe. */
    bool rbaExists(SpecimenId);

    /** Associate new RBA data with a specimen.
     *
     *  The ROSE Binary Analysis (RBA) data is read from the specified existing, readable file and copied into this database to
     *  be associated with the indicated specimen.  If the specimen had previous RBA data, the new data read from the file
     *  replaces the old data.  If any data cannot be copied from the file into the database then an @ref Exception is thrown
     *  and the database is not modified.  The specimen ID must be valid.
     *
     *  Thread safety: Not thread safe. */
    void saveRbaFile(const boost::filesystem::path&, SpecimenId);

    /** Extract RBA data from the database into a file.
     *
     *  The ROSE Binary Analysis (RBA) data associated with the indicated specimen is copied from the database into the
     *  specified file. The file is created if it doesn't exist, or truncated if it does exist. If the specimen does not have
     *  associated RBA data or if any data could not be copied to the file, then an @ref Exception is thrown. The specimen ID
     *  must be valid.
     *
     *  Thread safety: Not thread safe. */
    void extractRbaFile(const boost::filesystem::path&, SpecimenId);

    /** Remove any associated RBA data.
     *
     *  If the indicated specimen has ROSE Binary Analysis (RBA) data, then it is removed from the database.
     *
     *  Thread safety: Not thread safe. */
    void eraseRba(SpecimenId);

    //------------------------------------------------------------------------------------------------------------------------
    // Cached symbolic state.
    //------------------------------------------------------------------------------------------------------------------------

    /** Check whether a test case has a symbolic state.
     *
     *  Returns true if the indicated test case has an associated symbolic state, and false if it doesn't. */
    bool symbolicStateExists(TestCaseId);

    /** Save a symbolic state.
     *
     *  If the test case already has a symbolic state, then the old state is replaced by the specified state.  If the specified
     *  state is a null pointer, then any existing state is removed from the database. */
    void saveSymbolicState(TestCaseId, const InstructionSemantics::BaseSemantics::StatePtr&);

    /** Obtain the symbolic state from the database.
     *
     *  If the test case has no symbolic state then a null pointer is returned. */
    InstructionSemantics::BaseSemantics::StatePtr extractSymbolicState(TestCaseId);

    /** Remove the symbolic state for a test case.
     *
     *  This is a no-op if the test case doesn't have a symbolic state. */
    void eraseSymbolicState(TestCaseId);

    //------------------------------------------------------------------------------------------------------------------------
    // Cached concrete execution results. This is large data. Each test case has zero or one associated concrete results.
    //------------------------------------------------------------------------------------------------------------------------

    /** Check whether a test case has concrete results.
     *
     *  Returns true if the indicated test case has concrete results. Each test case can have zero or one set of concrete
     *  results. */
    bool concreteResultExists(TestCaseId);

    /** Associate concrete results with a test case.
     *
     *  The specified concrete execution results are associated with the specified test case, replacing any previous concrete
     *  results that might have been present for the test case. If the concrete results are null, then any existing concrete
     *  results for this test case are removed.
     *
     *  The concrete results are copied by this function. The caller continues to own the pointer. */
    void saveConcreteResult(const TestCasePtr&, const ConcreteResultPtr&);

    /** Read concrete results from the database.
     *
     *  Reads concrete results from the database and returns a pointer to them. If the test case has no concrete results then a
     *  null pointer is returned. */
    ConcreteResultPtr readConcreteResult(TestCaseId);

   /** Returns @p n test cases without concrete results.
    *
    * Thread safety: thread safe
    */
   std::vector<TestCaseId> needConcreteTesting(size_t n = UNLIMITED);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    /** Associate TestCase w/ TestSuite.
     *
     *  Normally a test case is associated with a test suite when the test case is created in the database by virtue of the
     *  database being scoped to a current test suite.  However, this function can be used to assign the test case to a
     *  different test suite. Both IDs must be valid.
     *
     * Thread safety: thread safe
     */
   void assocTestCaseWithTestSuite(TestCaseId, TestSuiteId);

   /** Returns @p n test cases without concolic results.
    *
    * Thread safety: thread safe
    */
   std::vector<TestCaseId> needConcolicTesting(size_t n = UNLIMITED);

#if 0 // [Robb Matzke 2020-07-15]
    // Use saveConcreteResults instead, which allows concrete results to also be removed.
   /** Updates a test case and its results.
    *
    * @param testCase a pointer to a test case
    * @param details  a polymorphic object holding results for a concrete execution
    *
    * Thread safety: thread safe
    */
   void insertConcreteResults(const TestCasePtr &testCase, const ConcreteResult& details);
#endif

   /** Tests if there are more test cases that require concrete or concolic testing.
    *
    * Thread safety: thread safe
    */
   bool hasUntested();

    /** Time stamp string. */
    static std::string timestamp();

private:
    static Ptr create(const std::string &url, const Sawyer::Optional<std::string> &testSuiteName);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
