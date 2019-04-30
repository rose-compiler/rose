
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


void runTestcase(concolic::Database::Ptr db, concolic::TestCaseId testcaseId)
{
  typedef boost::movelib::unique_ptr<concolic::ConcreteExecutor::Result> ExecutionResult;
  
  concolic::LinuxExecutorPtr exec     = concolic::LinuxExecutor::instance();
  concolic::TestCasePtr      testcase = db->object(testcaseId, concolic::Update::YES);  
  
  assert(testcase.getRawPointer());  
  std::cout << "dbtest: executing testcase " << testcase->name() << std::endl;
  ExecutionResult            result   = exec->execute(testcase);
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
  concolic::SpecimenId              grep2_bin = copyBinaryToDB(db, "/usr/bin/grep",      concolic::Update::YES);
  concolic::SpecimenId              xyz_bin   = copyBinaryToDB(db, "/usr/bin/xyz",       concolic::Update::YES);
  
  concolic::TestCaseId              ls_tst    = createTestCase(db, ls_bin, "ls");  
  if (!ls_tst) ls_tst = concolic::TestCaseId(1); // in case the db alreay existed
  
  concolic::TestCaseId              ls_la_tst = createTestCase(db, ls_bin, "ls -la", "-la");
  if (!ls_la_tst) ls_la_tst = concolic::TestCaseId(2); // in case the db alreay existed
  
  runTestcase(db, ls_tst);
  runTestcase(db, ls_la_tst);  
}

