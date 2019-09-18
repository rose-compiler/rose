
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

    enum specimenreuse { duplicate, unique, defaultreuse = unique };

    // database
    void connectdb(const char* db, const char* expect);
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
    InvocationDesc* invoke(const char* note, const char* specimen, Arguments* args) const;

    // removes quotes from the string
    char* unquoteString(const char* str);

    // test definition
    void test( const char*     suite,
               const char*     test,
               const char*     expct,
               Environment*    envp,
               InvocationDesc* invocation
             );

    void runTestcase(TestCaseId testcaseId, expectation expct);
    void runTest(const char* testsuitename, int cnt, const char* expect);

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
    Specimen::Ptr  specimen (const std::string& s, specimenreuse reuse);
    TestSuite::Ptr testSuite(const std::string& s, bool createMissingEntry = true);

  private:
    Database::Ptr db;
};


Crsh& crsh();


#endif /* CRSH_H */
