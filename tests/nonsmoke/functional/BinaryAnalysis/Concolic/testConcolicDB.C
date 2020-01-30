#include <rose.h>
#include <BinaryConcolic.h>
#include <Sawyer/CommandLine.h>
#include <boost/filesystem.hpp>
#include <SqlDatabase.h>

#include "configDB.h"

#if TEST_CONCOLICDB

namespace concolic = Rose::BinaryAnalysis::Concolic;

// Get the components of the $PATH environment variable.
static std::vector<boost::filesystem::path>
executionPaths() {
    if (const char *PATH = getenv("PATH")) {
        std::vector<std::string> parts = Rose::StringUtility::split(":", PATH);
        return std::vector<boost::filesystem::path>(parts.begin(), parts.end());
    }
    return std::vector<boost::filesystem::path>();
}

// Given a file name, try to find the file in one of the specified directories. But if an absolute name was specified, look
// only for the specified name. If the name exists (as a file, directory, link, etc) then return its path, otherwise return
// an empty path.
static boost::filesystem::path
findFile(const boost::filesystem::path &name, const std::vector<boost::filesystem::path> &directories) {
    if (name.is_absolute()) {
        return boost::filesystem::exists(name) ? name : boost::filesystem::path();
    } else {
        BOOST_FOREACH (const boost::filesystem::path &dir, directories) {
            boost::filesystem::path fullName = dir / name;
            if (boost::filesystem::exists(fullName))
                return fullName;
        }
    }

    return boost::filesystem::path();
}

/** creates a new Specimen object from the (binary) file @ref executableName
 *  and copies it over to the DB.
 */
concolic::SpecimenId
copyBinaryToDB( concolic::Database::Ptr db,
                const boost::filesystem::path& executableName,
                concolic::Update::Flag update = concolic::Update::YES
              )
{
  try
  {
    concolic::Specimen::Ptr binary = concolic::Specimen::instance(executableName);
    concolic::SpecimenId    id = db->id(binary, update);
    std::cout << "dbtest: copying over " << executableName
              << " new id: " << (id ? id.get() : -1)
              << " update: " << update
              << std::endl;

    return id;
  }
  catch (const SqlDatabase::Exception& e)
  {
    std::cout << "dbtest: copying over x" << executableName
              << " failed with: " << e.what()
              << std::endl;
  }
  catch (const std::logic_error& e)
  {
    std::cout << "dbtest: copying over" << executableName
              << " failed with: " << e.what()
              << std::endl;
  }
  catch (const std::runtime_error& e)
  {
    std::cout << "dbtest: copying over v" << executableName
              << " failed with: " << e.what()
              << std::endl;
  }

  return concolic::SpecimenId();
}

/** creates a new TestSuite object with name @ref n
 *  and stores it in the database @ref db.
 */
concolic::TestSuiteId
createTestSuite(concolic::Database::Ptr db, const std::string& n)
{
  try
  {
    concolic::TestSuitePtr ts = concolic::TestSuite::instance(n);
    concolic::TestSuiteId  id = db->id(ts, concolic::Update::YES);

    std::cout << "dbtest: creating testsuite " << n
              << " new id: " << id.get()
              << std::endl;

    return id;
  }
  catch (const SqlDatabase::Exception& e)
  {
    std::cout << "dbtest: creating testsuite " << n
              << " failed with: " << e.what()
              << std::endl;
  }
  catch (const std::logic_error& e)
  {
    std::cout << "dbtest: creating testsuite " << n
              << " failed with: " << e.what()
              << std::endl;
  }
  catch (const std::runtime_error& e)
  {
    std::cout << "dbtest: creating testsuite " << n
              << " failed with: " << e.what()
              << std::endl;
  }

  return concolic::TestSuiteId();
}


/** creates a new TestCase object for specimen @ref specimenId,
 *  name @ref n, and command line arguments @ref args.
 *  The testcase object is also stored in the database @ref db.
 */
concolic::TestCaseId
createTestCase( concolic::Database::Ptr db,
                concolic::SpecimenId specimenId,
                std::string n,
                const std::vector<std::string>& args
              )
{
  assert(specimenId);

  try
  {
    concolic::Specimen::Ptr specimen = db->object(specimenId, concolic::Update::NO);
    assert(specimen);

    concolic::SpecimenId    checking = db->id(specimen, concolic::Update::NO);
    assert(checking.get() == specimenId.get());

    concolic::TestCasePtr   testcase = concolic::TestCase::instance(specimen);

    testcase->name(n);
    testcase->args(args);

    concolic::TestCaseId    id       = db->id(testcase, concolic::Update::YES);

    std::cout << "dbtest: creating testcase "
              << " new id: " << id.get()
              << std::endl;

    return id;
  }
  catch (const SqlDatabase::Exception& e)
  {
    std::cout << "dbtest: creating testcase "
              << " failed with: " << e.what()
              << std::endl;
  }
  catch (const std::logic_error& e)
  {
    std::cout << "dbtest: creating testcase "
              << " failed with: " << e.what()
              << std::endl;
  }
  catch (const std::runtime_error& e)
  {
    std::cout << "dbtest: creating testcase "
              << " failed with: " << e.what()
              << std::endl;
  }

  return concolic::TestCaseId();
}

/** creates a new TestCase object for specimen @ref specimenId,
 *  name @ref n, and command line argument @ref arg.
 *  The testcase object is also stored in the database @ref db.
 *  A missing argument is distinct from a present argument whose value is an empty string.
 */
concolic::TestCaseId
createTestCase(concolic::Database::Ptr db, concolic::SpecimenId specimenId, std::string n) {
    return createTestCase(db, specimenId, n, std::vector<std::string>());
}

concolic::TestCaseId
createTestCase(concolic::Database::Ptr db, concolic::SpecimenId specimenId, std::string n,
                const std::string &arg) {
    return createTestCase(db, specimenId, n, std::vector<std::string>(1, arg));
}

concolic::TestCaseId
createTestCase(concolic::Database::Ptr db, concolic::SpecimenId specimenId, std::string n,
               const std::string &arg0, const std::string &arg1) {
    std::vector<std::string> args;
    args.push_back(arg0);
    args.push_back(arg1);
    return createTestCase(db, specimenId, n, args);
}

/** Makes testcase @ref tc a member of the testsuite @ref tc.
 *  The relationship is stored in the database @ref db.
 */
void addTestToSuite( concolic::Database::Ptr db,
                     concolic::TestCaseId tc,
                     concolic::TestSuiteId ts
                   )
{
  try
  {
    db->assocTestCaseWithTestSuite(tc, ts);

    std::cout << "dbtest: assoc'd test w/ suite "
              << std::endl;

  }
  catch (const SqlDatabase::Exception& e)
  {
    std::cout << "dbtest: assoc test w/ suite "
              << " failed with: " << e.what()
              << std::endl;
  }
  catch (const std::logic_error& e)
  {
    std::cout << "dbtest: assoc test w/ suite "
              << " failed with: " << e.what()
              << std::endl;
  }
  catch (const std::runtime_error& e)
  {
    std::cout << "dbtest: assoc test w/ suite "
              << " failed with: " << e.what()
              << std::endl;
  }
}

/** Runs the testcase @ref testcaseId.
 */
void runTestcase(concolic::Database::Ptr db, concolic::TestCaseId testcaseId)
{
  typedef std::auto_ptr<concolic::ConcreteExecutor::Result> ExecutionResult;

  concolic::LinuxExecutorPtr exec     = concolic::LinuxExecutor::instance();
  concolic::TestCasePtr      testcase = db->object(testcaseId, concolic::Update::YES);

  assert(testcase.getRawPointer());
  std::cout << "dbtest: executing testcase " << testcase->name() << std::endl;
  ExecutionResult            result(exec->execute(testcase));
}

/** Functor to run a new testcase.
 */
struct TestCaseStarter
{
  concolic::Database::Ptr db;

  explicit
  TestCaseStarter(concolic::Database::Ptr database)
  : db(database)
  {}

  void operator()(concolic::TestCaseId id)
  {
    runTestcase(db, id);
  }
};

/** Runs all testcases in the database @ref db.
 *  If set in @ref db, the testcases are limited to the current
 *  testsuite.
 */

void testAllTestCases(concolic::Database::Ptr db)
{
  std::vector<concolic::TestCaseId> tests = db->testCases();

  std::for_each(tests.begin(), tests.end(), TestCaseStarter(db));
}


// copied from Database.C
std::string extract_filename(std::string url)
{
  static const std::string locator = "sqlite3://";

  if (!boost::starts_with(url, locator)) return url;

  size_t limit = url.find_first_of('?', locator.size());

  return url.substr(locator.size(), limit);
}


/** runs through a number of tests.
 */
void testAll(std::string dburi)
{
  concolic::Database::Ptr db  = concolic::Database::create(dburi);

  // add new file(s)
  concolic::SpecimenId              tst       = copyBinaryToDB(db, "testConcolicDB", concolic::Update::YES);
  assert(tst);

  concolic::SpecimenId              ls_bin    = copyBinaryToDB(db, findFile("ls", executionPaths()), concolic::Update::YES);
  assert(ls_bin);
  // storing a specimen multiple times is allowed
  // if (!ls_bin) ls_bin = concolic::SpecimenId(2); // in case the db alreay existed

  concolic::SpecimenId              ls2_bin   = copyBinaryToDB(db, findFile("ls", executionPaths()), concolic::Update::YES);
  assert(ls2_bin);

  concolic::SpecimenId              grep_bin  = copyBinaryToDB(db, findFile("grep", executionPaths()), concolic::Update::NO );
  assert(!grep_bin); /* update is NO, thus no object is created. */

  concolic::SpecimenId              more_bin  = copyBinaryToDB(db, findFile("more", executionPaths()), concolic::Update::YES);
  assert(more_bin);
  // storing a specimen multiple times is allowed
  // if (!more_bin) more_bin = concolic::SpecimenId(5); // in case the db alreay existed

  ASSERT_forbid(boost::filesystem::exists("/usr/bin/xyz"));
  concolic::SpecimenId              xyz_bin   = copyBinaryToDB(db, "/usr/bin/xyz", concolic::Update::YES);
  assert(!xyz_bin);

  // define test cases
  concolic::TestCaseId              ls_tst    = createTestCase(db, ls_bin, "ls", "/");
  assert(ls_tst);
  // test-case names are not unique
  // if (!ls_tst) ls_tst = concolic::TestCaseId(1); // in case the db already existed

  concolic::TestCaseId              ls_la_tst = createTestCase(db, ls_bin, "ls -la", "-la", "/");
  assert(ls_la_tst);
  // test-case names are not unique
  // if (!ls_la_tst) ls_la_tst = concolic::TestCaseId(2); // in case the db already existed

  concolic::TestCaseId              more_tst = createTestCase(db, ls_bin, "more", "y.txt");
  assert(more_tst);
  // test-case names are not unique
  // if (!more_tst) more_tst = concolic::TestCaseId(3); // in case the db already existed

  runTestcase(db, ls_tst);
  runTestcase(db, ls_la_tst);
  runTestcase(db, more_tst);

  // define test suites
  concolic::TestSuiteId             ls_suite  = createTestSuite(db, "ls family");
  if (!ls_suite) ls_suite = concolic::TestSuiteId(1);

  addTestToSuite(db, ls_la_tst, ls_suite);
  addTestToSuite(db, ls_tst, ls_suite);

  addTestToSuite(db, concolic::TestCaseId(), ls_suite);

  concolic::TestSuiteId             more_suite = createTestSuite(db, "more family");
  if (!more_suite) more_suite = concolic::TestSuiteId(2);

  addTestToSuite(db, more_tst, more_suite);
  testAllTestCases(db); // w/o test suite set

  db->testSuite(db->object(more_suite));
  testAllTestCases(db); // w/ test suite set

  std::cout << "dbtest: ** rba tests" << std::endl;

  if (db->rbaExists(more_bin))
  {
    std::cout << "dbtest: has rba" << std::endl;

    db->extractRbaFile("./old.rba", more_bin);
    std::cout << "dbtest: extracted rba" << std::endl;

    db->eraseRba(more_bin);
    std::cout << "dbtest: erased rba" << std::endl;
  }

  db->saveRbaFile(findFile("more", executionPaths()), more_bin);
  std::cout << "dbtest: stored rba" << std::endl;
}

void cleanup(std::string fileUri)
{
  boost::filesystem::remove("./old.rba");
  boost::filesystem::remove(extract_filename(fileUri));
}

#endif /* TEST_CONCOLICDB */


int main(int argc, char** argv)
{
#if TEST_CONCOLICDB
  static const std::string defaultDBUri = "sqlite3://./test.db";

  std::string dbUri = (argc > 1 ? std::string(argv[1]) : defaultDBUri);

  // prepare a new test environment by removing any stale
  //  database file.
  cleanup(dbUri);

  // we  w/ a fresh DB
  testAll(dbUri);

  // rerun with existing DB
  testAll(dbUri);

  // cleanup all files that were generated in the process.
  cleanup(dbUri);
#endif /* TEST_CONCOLICDB */

  return 0;
}
