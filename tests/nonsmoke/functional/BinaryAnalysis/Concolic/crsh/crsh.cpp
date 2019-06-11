#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "crsh.hpp"

#include "sage3basic.h"
#include "BinaryConcolic.h"

#include "crsh-parse.hpp"


struct yy_buffer_state;
extern FILE* yyin;
typedef yy_buffer_state* YY_BUFFER_STATE;

YY_BUFFER_STATE yy_scan_string(const char * str); // it does not work.
YY_BUFFER_STATE yy_scan_buffer(char *, size_t);
void yy_delete_buffer(YY_BUFFER_STATE buffer);
void yy_switch_to_buffer(YY_BUFFER_STATE buffer);

extern char* yytext;
extern int yylex (void);


namespace concolic = Rose::BinaryAnalysis::Concolic;

static inline
std::string
rtrim(const std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
  const size_t pos = str.find_last_not_of(chars);

  if (pos == std::string::npos) return str;

  return str.substr(0, pos+1);
}

//! crush
//! - to press between opposing bodies so as to break, compress, or injure
struct Crsh
{
    void connect(const std::string& s);
    void disconnect();

    void suite(const std::string& s);
    void suite_end();

    void test(const std::string& s);
    void test_end();

    void cmdlarg(const std::string& s);
    void invoke(const std::string& s);
    void envvar(const concolic::EnvValue& v);

    void runTestcase(concolic::TestCaseId testcaseId);
    void run(const char* testsuitename, int cnt);

    void parse();

    std::ostream& out() { return std::cout; }
    std::ostream& err() { return std::cerr; }

  private:
    concolic::Specimen::Ptr  specimen (const std::string& s);
    concolic::TestSuite::Ptr testSuite(const std::string& s);

  private:
    concolic::DatabasePtr              db;
    concolic::TestSuitePtr             current_suite;
    std::vector<concolic::TestCasePtr> tests;
    std::vector<std::string>           names;
    std::vector<concolic::EnvValue>    environment;
    std::vector<std::string>           args;
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

  free(const_cast<char*>(str));
  return res;
}

void crsh_db(const char* str)      { crsh.connect(conv(str)); }
void crsh_suite(const char* str)   { crsh.suite(conv(str)); }
void crsh_test(const char* str)    { crsh.test(conv(str)); }
void crsh_cmdlarg(const char* str) { crsh.cmdlarg(conv(str)); }
void crsh_invoke(const char* str)  { crsh.invoke(conv(str)); }

void crsh_envvar(const char* key, const char* val)
{
  crsh.envvar(concolic::EnvValue(conv(key), conv(val)));
}

void crsh_db_end()    { crsh.disconnect(); }
void crsh_suite_end() { crsh.suite_end(); }
void crsh_test_end()  { crsh.test_end();  }

void crsh_run(const char* testsuite, int num)
{
  crsh.run(testsuite, num);
}


//
// Crsh implementation

void Crsh::disconnect()
{
  db = Sawyer::Nothing();
}

void Crsh::connect(const std::string& s)
{
  db = concolic::Database::instance(s);
}

template <class T>
static
typename T::Ptr
byName(concolic::DatabasePtr& db, const std::string& s, concolic::ObjectId<T> id)
{
  if (id) return db->object(id);

  // if not in the database already, create it from a file
  typename T::Ptr obj = T::instance(s);

  db->id(obj);
  return obj;
}

concolic::TestSuite::Ptr
Crsh::testSuite(const std::string& s)
{
  return byName(db, s, db->testSuite(s));
}


void Crsh::suite(const std::string& s)
{
  current_suite = testSuite(s);
}

void Crsh::suite_end()
{
  // store all in DB
  concolic::TestSuiteId id = db->testSuite(current_suite);

  // std::for_each(tests.begin(), tests.end(), ..);

  tests.clear();
  current_suite = Sawyer::Nothing();
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

void Crsh::envvar(const concolic::EnvValue& v)
{
  environment.push_back(v);
}

concolic::SpecimenPtr
Crsh::specimen(const std::string& specimen_name)
{
  return byName(db, specimen_name, db->specimen(specimen_name));
}

void Crsh::test_end()
{
  assert(names.size() == 2);

  // get the object for the specimen's name
  concolic::SpecimenPtr specobj = specimen(pop(names));

  // create a new test
  concolic::TestCasePtr test    = concolic::TestCase::instance(specobj);

  test->name(pop(names));
  test->args(args);
  test->env(environment);

  // store in database
  db->id(test);

  // store for later execution
  tests.push_back(test);

  // clear the intermediate storage
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


void Crsh::run(const char* testsuite, int num)
{
  typedef std::vector<concolic::TestCaseId> test_container;

  if (num < 0) num = 1;

  if (testsuite != NULL)
  {
    std::string tstsuite = conv(testsuite);

    concolic::TestSuiteId  id  = db->testSuite(testsuite);
    concolic::TestSuitePtr ptr = db->object(id);

    db->testSuite(ptr);
  }

  test_container tests = db->needConcreteTesting(num);

  std::for_each(tests.begin(), tests.end(), TestCaseStarter(*this));
}



//
// command line interface

static
char lastCh(const std::string& s, char empty = 0)
{
  if (s.size() == 0) return empty;

  return s[s.size() - 1];
}

static
std::string readLine(std::istream& input)
{
  std::string res;

  getline(input, res);
  return res;
}

void Crsh::parse()
{
  try
  {
    yyparse();
  }
  catch (const std::logic_error& e)
  {
    err() << e.what() << std::endl;
    exit(0);
  }
  catch (const std::runtime_error& e)
  {
    err() << e.what() << std::endl;
  }
}

void readEvalPrint(std::istream& input)
{
  static const char  CONTINUE_ON_NEXT_LINE = '\\';
  static const char* EXIT_CMD  = "exit";

  crsh.out() << "crsh v0.0.1\n" << std::endl;

  std::string cmd;

  while (!input.eof() && EXIT_CMD != cmd)
  {
    crsh.out() << ">";
    cmd = CONTINUE_ON_NEXT_LINE;

    // create input from multi-line input
    while (!input.eof() && CONTINUE_ON_NEXT_LINE == lastCh(cmd))
    {
      cmd = cmd.substr(0, cmd.size()-1);
      cmd += rtrim(readLine(input));
    }

    YY_BUFFER_STATE buf = yy_scan_string(cmd.c_str());
    crsh.parse();
    yy_delete_buffer(buf);
  }
}

void parse_file(const char* filename)
{
  yyin = fopen(filename, "r");

/*
  int token;
  while ((token = yylex()) != 0)
      printf("Token: %d (%s)\n", token, yytext);
*/
  crsh.parse();
  fclose(yyin);
}

int main(int argc, char** argv)
{
  if (argc == 1)
  {
    readEvalPrint(std::cin);
  }
  else
  {
    parse_file(argv[1]);
  }

  crsh.disconnect();
  return 0;
}
