
#ifndef CRSH_H
#define CRSH_H 1

#include <vector>
#include <string>

#include "sage3basic.h"
#include "BinaryConcolic.h"


typedef std::list<Rose::BinaryAnalysis::Concolic::EnvValue> Environment;
typedef std::list<std::string>                              Arguments;
typedef std::list<std::string>                              Annotations;

struct InvocationDesc;

struct Crsh
{
    typedef Rose::BinaryAnalysis::Concolic::Database    Database;
    typedef Rose::BinaryAnalysis::Concolic::TestCase    TestCase;
    typedef Rose::BinaryAnalysis::Concolic::TestCaseId  TestCaseId;
    typedef Rose::BinaryAnalysis::Concolic::TestSuite   TestSuite;
    typedef Rose::BinaryAnalysis::Concolic::TestSuiteId TestSuiteId;
    typedef Rose::BinaryAnalysis::Concolic::Specimen    Specimen;
    typedef Rose::BinaryAnalysis::Concolic::SpecimenId  SpecimenId;
    typedef Rose::BinaryAnalysis::Concolic::EnvValue    EnvValue;

    // representation of annotations   
    enum annotation_desc
    {
      // nothing
      none                   = 0,
      
      // expectation
      expect_success         = 1 << 0,      
      expect_failure         = 1 << 1,
      expect_default         = none, /* either result is OK */
      expect_all             = expect_success | expect_failure,
      
      // execution results alias expectations
      execute_success        = expect_success,
      execute_failure        = expect_failure,
      
      // reuse a unique specimen with the same name in the DB 
      reuse_unique           = 1 << 2,
      reuse_duplicate        = 1 << 3,
      reuse_default          = reuse_unique, /* maybe use duplicate to align behavior with the rest of concolic testing */
      reuse_all              = reuse_unique | reuse_duplicate,
      
      // address randomization 
      addr_randomize         = 1 << 4,
      addr_norandomize       = 1 << 5,
      addr_randomize_default = addr_norandomize, /* off for reproducible results */
      addr_randomize_all     = addr_randomize | addr_norandomize,
      
      // everything
      all                    = expect_all | reuse_all | addr_randomize_all       
    };
    
    // ctor
    Crsh()
    : db(), execmon()
    {}

    // database
    void connectdb(Annotations* expect, const char* db);
    void createdb(const char* db);
    void createdb(const char* db, const char* testsuite);
    void closedb();

    // environment
    EnvValue* envvar(const char* key, const char* value) const;

    Environment* environment() const;
    Environment* environment(Environment* env, const EnvValue* val) const;

    // command line arguments
    std::string* arg(const char*) const;
    std::string* arg(int) const;

    Arguments* args() const;
    Arguments* args(Arguments* arglst, const std::string* val) const;
 
    // annotation processing
    std::string* annotation(const char*) const;
    
    Annotations* annotations() const;
    Annotations* annotations(Annotations* annotations, const std::string* note) const;
    
    // invocation description
    InvocationDesc* invoke(Annotations* notes, const char* specimen, Arguments* args) const;

    // removes quotes from the string
    char* unquoteString(const char* str);
    
    // set the execution monitor
    void setMonitor(const char* str);

    // run the individual steps for a test
    void test(const char *suite, const char *test, Annotations *expct, Environment *envp, InvocationDesc *invocation);
    void testNoCatch(const std::string &suite, const std::string &test, Annotations *expct, Environment *envp, InvocationDesc *invocation);

    void runTestcase(TestCaseId testcaseId, annotation_desc);
    void runTest(Annotations*, const char* testsuitename, int cnt);

    // immediately invokes the described program
    void execute(InvocationDesc* invocation);

    void parse();

    std::ostream& out() const { return std::cout; }
    std::ostream& err() const { return std::cerr; }

  private:
    //! Returns  a specimen for the given name.
    //! \param   s the name of the specimen
    //! \param   reuse controls if a specimen in the database may be reused
    //! \throws  an std::runtime_error if reuse == unique and multiple specimens
    //!          with the same name already exist in the DB.
    //! \details If reuse is set to @ref unique (the default), crsh reuses a specimen
    //!          entry if the name occurs a single time (unique) in the DB.
    //!          If multiple specimen with the same name are present an exception
    //!          is thrown.
    //!          If no specimen with the given name exists or reuse is set to
    //!          @ref duplicate a new entry in the DB is created.
    Specimen::Ptr  specimen (const std::string& s, annotation_desc reuse);
    TestSuite::Ptr testSuite(const std::string& s, bool createMissingEntry = true);

  private:
    Database::Ptr db;
    std::string   execmon;
};


Crsh& crsh();


#endif /* CRSH_H */
