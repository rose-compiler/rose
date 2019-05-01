
#include "rose.h"
#include "BinaryConcolic.h"

namespace concolic = Rose::BinaryAnalysis::Concolic;

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
              << " new id: " << id.get()
              << std::endl;

    return id;
  }
  catch (const SqlDatabase::Exception& e)
  {
    std::cout << "dbtest: copying over " << executableName 
              << " failed with: " << e.what()
              << std::endl;
  }    
  catch (const std::logic_error& e)
  {
    std::cout << "dbtest: copying over " << executableName 
              << " failed with: " << e.what()
              << std::endl;        
  }    
  catch (const std::runtime_error& e)
  {
    std::cout << "dbtest: copying over " << executableName 
              << " failed with: " << e.what()
              << std::endl;    
  }
  
  return concolic::SpecimenId();
}

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

concolic::TestCaseId 
createTestCase( concolic::Database::Ptr db, 
                concolic::SpecimenId specimenId, 
                std::string n,
                const std::vector<std::string>& args                
              )
{
  try
  {
    concolic::Specimen::Ptr specimen = db->object(specimenId, concolic::Update::NO);        
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

concolic::TestCaseId 
createTestCase( concolic::Database::Ptr db, 
                concolic::SpecimenId specimenId, 
                std::string n,
                std::string arg = ""
              )
{
  std::vector<std::string> args;
  
  if (arg.size() != 0) args.push_back(arg);
  return createTestCase(db, specimenId, n, args);
}


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


void runTestcase(concolic::Database::Ptr db, concolic::TestCaseId testcaseId)
{
  typedef std::auto_ptr<concolic::ConcreteExecutor::Result> ExecutionResult;
  
  concolic::LinuxExecutorPtr exec     = concolic::LinuxExecutor::instance();
  concolic::TestCasePtr      testcase = db->object(testcaseId, concolic::Update::YES);  
  
  assert(testcase.getRawPointer());  
  std::cout << "dbtest: executing testcase " << testcase->name() << std::endl;
  ExecutionResult            result(exec->execute(testcase));
}

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

void testAllTestCases(concolic::Database::Ptr db)
{
  std::vector<concolic::TestCaseId> tests = db->testCases();
  
  std::for_each(tests.begin(), tests.end(), TestCaseStarter(db));
}


int main()
{
  std::string             dburi = "sqlite3://tmp/test.db";
  //~ std::string             dburl = SqlDatabase::Connection::connectionSpecification(dburi, SqlDatabase::SQLITE3);
  
  concolic::Database::Ptr db  = concolic::Database::instance(dburi);
  
  // add new file(s)
  concolic::SpecimenId              tst       = copyBinaryToDB(db, "testBinaryConcolic", concolic::Update::YES);
  concolic::SpecimenId              ls_bin    = copyBinaryToDB(db, "/usr/bin/ls",        concolic::Update::YES);
  if (!ls_bin) ls_bin = concolic::SpecimenId(2); // in case the db alreay existed
  
  concolic::SpecimenId              ls2_bin   = copyBinaryToDB(db, "/usr/bin/ls",        concolic::Update::YES);
  concolic::SpecimenId              grep_bin  = copyBinaryToDB(db, "/usr/bin/grep",      concolic::Update::NO );    
  concolic::SpecimenId              more_bin  = copyBinaryToDB(db, "/usr/bin/more",      concolic::Update::YES);
  
  //~ std::cout << "more** " << more_bin.get() << std::endl;
  if (!more_bin) more_bin = concolic::SpecimenId(3); // in case the db alreay existed
  
  concolic::SpecimenId              xyz_bin   = copyBinaryToDB(db, "/usr/bin/xyz",       concolic::Update::YES);

  // define test cases  
  concolic::TestCaseId              ls_tst    = createTestCase(db, ls_bin, "ls");  
  if (!ls_tst) ls_tst = concolic::TestCaseId(1); // in case the db already existed
  
  concolic::TestCaseId              ls_la_tst = createTestCase(db, ls_bin, "ls -la", "-la");
  if (!ls_la_tst) ls_la_tst = concolic::TestCaseId(2); // in case the db already existed
  
  concolic::TestCaseId              more_tst = createTestCase(db, ls_bin, "more", "y.txt");
  if (!more_tst) more_tst = concolic::TestCaseId(3); // in case the db already existed
    
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
}

