
#ifndef CRSH_H
#define CRSH_H 1

#include <vector>
#include <string>

#include "sage3basic.h"
#include "BinaryConcolic.h"


typedef std::list<Rose::BinaryAnalysis::Concolic::EnvValue> Environment;
typedef std::list<std::string>                              Arguments;

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

    enum expectation { none, success, failure };

    // database
    void connectdb(const char* db, expectation);
    void createdb(const char* db);
    void createdb(const char* db, const char* testsuite);
    void closedb();

    // environment
    EnvValue* envvar(const char* key, const char* value) const;

    Environment* environment() const;
    Environment* environment(Environment* env, const EnvValue* val) const;

    // command line arguments
    std::string* arg(const char* key) const;

    Arguments* args() const;
    Arguments* args(Arguments* arglst, const std::string* val) const;

    // invocation description
    InvocationDesc* invoke(const char*, Arguments* args) const;

    // removes quotes from the string
    char* unquoteString(const char* str);

    // test definition
    void test( const char*     suite,
               const char*     test,
               expectation     expct,
               Environment*    envp,
               InvocationDesc* invocation
             );

    void runTestcase(TestCaseId testcaseId, expectation expct);
    void runTest(const char* testsuitename, int cnt, expectation expct);

    // immediately invokes the described program
    void execute(InvocationDesc* invocation);

    // converts a string into an annotation
    expectation annotate(const char*);

    void parse();

    std::ostream& out() const { return std::cout; }
    std::ostream& err() const { return std::cerr; }

  private:
    Specimen::Ptr  specimen (const std::string& s);
    TestSuite::Ptr testSuite(const std::string& s, bool createMissingEntry = true);

  private:
    Database::Ptr db;
};


Crsh& crsh();


#endif /* CRSH_H */
