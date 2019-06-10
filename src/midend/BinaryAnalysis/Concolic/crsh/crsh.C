#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "crsh.h"

#include "BinaryConcolic.h"

namespace conc = Rose::BinaryAnalysis::Concolic;

static inline
std::string
rtrim(const std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
  const size_t pos = str.find_last_not_of(chars);

  if (pos == std::npos) return str;

  return str.substr(0, pos+1);
}

struct Crsh
{
    void opendb(const std::string& s);
    void closedb();

    void suite(const std::string& s);
    void suite_end();

    void test(const std::string& s);
    void test_end();

    void cmdlarg(const std::string& s);
    void invoke(const std::string& s);
    void envvar(const conc::EnvValue& v);

    conc::SpecimenPtr specimen(const std::string& s);
    void runTestcase(concolic::TestCaseId testcaseId);
    void run(int num, const char* testsuitename);

    std::ostream& out() { return std::cout; }

  private:
    conc::DatabasePtr              db;
    conc::TestSuitePtr             suite;
    std::vector<conc::TestCasePtr> tests;
    std::vector<std::string>       names;
    std::vector<EnvValue>          environment;
    std::vector<string>            args;
};

Crsh crsh;

namespace
{
  template <class T>
  T pop(std::vector<T>& vec)
  {
    T res = vec.back();

    vec.pop_back();
    return res;
  }
}

//
// Bison interface

static inline
std::string conv(const char* str)
{
  std::string res(str);

  free(str);
  return res;
}

void crsh_db(const char* str)      { crsh.db(conv(str)); }
void crsh_suite(const char* str)   { crsh.suite(conv(str)); }
void crsh_test(const char* str)    { crsh.test(conv(str)); }
void crsh_cmdlarg(const char* str) { crsh.cmdlagr(conv(str)); }
void crsh_invoke(const char* str)  { crsh.invoke(conv(str)); }

void crsh_envvar(const char* key, const char* val)
{
  crsh.envvar(conc::EnvValue(conv(key), conv(val)));
}

void crsh_db_end()    { crsh.db_end();    }
void crsh_suite_end() { crsh.suite_end(); }
void crsh_test_end()  { crsh.test_end();  }


//
// Crsh implementation

void Crsh::db_end()
{
  db.clear();
}

void Crsh::db(const std::string& s)
{
  db = conc::Database::instance(s);
}

void Crsh::suite(const std::string& s)
{
  db = conc::Database::instance(s);
}

void Crsh::suite_end()
{
  // store all in DB
  conc::TestSuiteId id = db->testSuite(suite);

  std::for_each(tests.begin(), tests.end(), ..);

  tests.clear();
  suite.clear();
}

void Crsh::test(const std::string& s)
{
  names.push_back(s);

  assert(environment.size() == 0);
  assert(args.size() == 0);
}

void Crsh::cmdlarg(const std::string& s)
{
  args.push_back(s);
}

void Crsh::envvar(const conc::EnvValue& v)
{
  environment.push_back(v);
}

conc::SpecimenPtr Crsh::specimen(const std::string& s)
{
  conc::SpecimenId id = db->specimen(s);

  if (id) return db->object(id);

  return conc::Specimen::instance(s);
}

void Crsh::test_end()
{
  assert(names.size() == 2);

  conc::SpecimenPtr specimen = specimen(pop(names));
  conc::TestCasePtr test     = conc::TestCase::instance(specimen);

  test.name(pop(names));
  test.args(args);
  test.envs(environment);

  tests.push_back(test);

  environment.clear();
  args.clear();
  assert(names.size() == 0);
}

void Crsh::invoke(const std::string& s)
{
  names.push_back(s);
}


/** Runs the testcase @ref testcaseId.
 */
void Crsh::runTestcase(concolic::TestCaseId testcaseId)
{
  typedef std::auto_ptr<concolic::ConcreteExecutor::Result> ExecutionResult;

  concolic::LinuxExecutorPtr exec     = concolic::LinuxExecutor::instance();
  concolic::TestCasePtr      testcase = db->object(testcaseId, concolic::Update::YES);

  assert(testcase.getRawPointer());
  std::cout << "dbtest: executing testcase " << testcase->name() << std::endl;
  ExecutionResult            result(exec->execute(testcase));

  db->insertConcreteResults(testcase, *result.get());
}


/** Functor to run a new testcase.
 */
struct TestCaseStarter
{
  Crsh& crsh;

  explicit
  TestCaseStarter(Crsh& crshobj)
  : crsh(crshobj)
  {}

  void operator()(concolic::TestCaseId id)
  {
    crsh.runTestcase(id);
  }
};


void testAllTestCases(concolic::Database::Ptr db)
{
  std::vector<concolic::TestCaseId> tests = db->testCases();

  std::for_each(tests.begin(), tests.end(), TestCaseStarter(db));
}

void Crsh::run(int num, const char* testsuite)
{
  typedef std::vector<Database::TestCaseId> test_container;

  if (num < 0) num = 1;

  if (testsuite != 0)
  {
    std::string tstsuite = conv(testsuite);

    TestSuiteId  id  = db->testsuite(testsuite);
    TestSuitePtr ptr = db->object(id);

    db->testSuite(ptr);
  }

  test_container tests = db->needConcreteTesting(num);

  std::for_each(tests.begin(), tests.end(), TestCaseStarter(*this));
}



//
// command line interface

void read_eval_print(std::istream& input)
{
  static const char  SLASH = '\\';
  static const char* EXIT  = "exit";

  crsh.out() << "crsh v0.0.1\n" << std::endl;

  std::string cmd;

  while (!is.eof() && EXIT != cmd)
  {
    crsh.out() << ">";
    cmd = SLASH;

    do
    {
      cmd = cmd.substr(0, cmd.size()-1);
      cmd += rtrim(getline(std::cin));
    } while (!is.eof() && cmd.size() > 0 && SLASH == cmd[cmd.size()-1])

    YY_BUFFER_STATE buf = yy_scan_string(cmd.c_str());
    yyparse();
    yy_delete_buffer(buf);
  }
}

void parse_file(const char* filename)
{
  yyin = fopen(filename, "r");
  yyparse();
  fclose(yyin);
}

int main(int argc, char** argv)
{
  if (argc == 1)
  {
    read_eval_print(std::cin);
  }
  else
  {
    parse_file(argv[1]);
  }

  crsh.db_end();
  return 0;
}
