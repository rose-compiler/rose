#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "crsh.hpp"

#include "sage3basic.h"
#include "BinaryConcolic.h"

#include "crsh-parse.hpp"

#include "../configDB.h"


//
// flex & bison declarations

struct yy_buffer_state;
extern FILE* yyin;
typedef yy_buffer_state* YY_BUFFER_STATE;

YY_BUFFER_STATE yy_scan_string(const char * str); // it does not work.
YY_BUFFER_STATE yy_scan_buffer(char *, size_t);
void yy_delete_buffer(YY_BUFFER_STATE buffer);
void yy_switch_to_buffer(YY_BUFFER_STATE buffer);

extern char* yytext;
extern int yylex (void);
extern int yyparse(void);


//
// InvocationDesc

//! InvocationDesc is opaque for other translation units

struct InvocationDesc
{
  std::string specimen;
  Arguments   arguments;

  InvocationDesc(const std::string& sp, const Arguments& args)
  : specimen(sp), arguments(args)
  {}
};

//
// auxiliary functions

std::ostream& operator<<(std::ostream& os, Crsh::TestCase& test)
{
  std::vector<std::string> args(test.args());

  os << test.name() << ": " << test.specimen()->name();

  for (size_t i = 0; i < args.size(); ++i)
    os << ' ' << args.at(i);

  return os;
}

static inline
std::string
rtrim(const std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
  const size_t pos = str.find_last_not_of(chars);

  if (pos == std::string::npos) return str;

  return str.substr(0, pos+1);
}

static inline
std::string conv(const char* str)
{
  std::string res(str);

  free(const_cast<char*>(str));
  return res;
}

template <class T>
static
typename T::Ptr
byName(Crsh::Database::Ptr& db, const std::string& s, Rose::BinaryAnalysis::Concolic::ObjectId<T> id)
{
  if (id) return db->object(id);

  // if not in the database already, create it from a file
  typename T::Ptr obj = T::instance(s);

  db->id(obj);
  return obj;
}

//
template <class T>
std::list<T>* enlist(std::list<T>* lst, const T* el)
{
  ROSE_ASSERT(lst && el);

  std::auto_ptr<const T> elguard(el);

  lst->push_back(*el);
  return lst;
}


void require_outcome(Crsh::expectation expct, Crsh::expectation outcome)
{
  if (expct == Crsh::none || expct == outcome)
    return;


}


//
// Crsh implementation

void Crsh::disconnect()
{
  db = Sawyer::Nothing();
}

void Crsh::connect(const std::string& s)
{
  db = Database::instance(s);
}

Crsh::TestSuite::Ptr
Crsh::testSuite(const std::string& s)
{
  return byName(db, s, db->testSuite(s));
}

Crsh::EnvValue*
Crsh::envvar(const char* key, const char* val) const
{
  return new EnvValue(conv(key), conv(val));
}

Environment*
Crsh::environment() const
{
  return new Environment();
}

Environment*
Crsh::environment(Environment* env, const EnvValue* val) const
{
  return enlist(env, val);
}


std::string*
Crsh::arg(const char* argument) const
{
  std::string tmp = conv(argument);

  return new std::string(tmp);
}

void Crsh::echo(const char* what)
{
  out() << conv(what) << std::endl;
}


void Crsh::echo_var(const char* id)
{
  std::string varid = conv(id);

  if ("concolicdb" == varid)
    Rose::BinaryAnalysis::Concolic::writeDBSchema(out());
  else if ("concolicsql" == varid)
    Rose::BinaryAnalysis::Concolic::writeSqlStmts(out());
  else
    out() << "unknown variable";

  out() << std::endl;
}

Crsh::expectation
Crsh::annotate(const char* expect)
{
  std::string note = conv(expect);

  if ("success" == note) return success;
  if ("failure" == note) return failure;

  std::cerr << "unexpected note: " << note << std::endl;
  exit(1);
}

char* Crsh::unquote_string(const char* str)
{
  size_t len   = strlen(str)-1;
  ROSE_ASSERT(len > 0 && str[0] == '\"');

  char*  clone = static_cast<char*>(malloc(len));
  strncpy(clone, str+1, len);
  ROSE_ASSERT(clone[len] == 0);

  free(const_cast<char*>(str));
  return clone;
}

Arguments*
Crsh::args() const
{
  return new Arguments();
}

Arguments*
Crsh::args(Arguments* arglst, const std::string* argument) const
{
  return enlist(arglst, argument);
}

InvocationDesc*
Crsh::invoke(const char* specimen, Arguments* args) const
{
  std::auto_ptr<Arguments> arguments(args);

  return new InvocationDesc(conv(specimen), *args);
}

template <class T>
static inline
std::vector<T> mkVector(const std::list<T>& lst)
{
  return std::vector<T>(lst.begin(), lst.end());
}

std::string str(Crsh::expectation expct)
{
  std::string res;

  switch (expct)
  {
    case Crsh::none:    res = "none";    break;
    case Crsh::success: res = "success"; break;
    case Crsh::failure: res = "failure"; break;
    default: ROSE_ASSERT(false);
  }

  return res;
}

void
Crsh::test(const char* ts, const char* tst, expectation exp, Environment* env, InvocationDesc* inv)
{
  std::string                   suitename = conv(ts);
  std::string                   testname  = conv(tst);
  std::auto_ptr<Environment>    envguard(env);
  std::auto_ptr<InvocationDesc> invguard(inv);
  expectation                   state = success;

  try
  {
    // get the object for the specimen's name
    Specimen::Ptr               specobj = specimen(inv->specimen);
    TestCase::Ptr               test    = TestCase::instance(specobj);

    test->name(testname);
    test->args(mkVector(inv->arguments));
    test->env(mkVector(*env));

    TestCaseId                  id        = db->id(test);
    TestSuite::Ptr              suite_obj = testSuite(suitename);
    TestSuiteId                 suite_id  = db->id(suite_obj);

    ROSE_ASSERT(id);
    ROSE_ASSERT(suite_id);

    db->assocTestCaseWithTestSuite(id, suite_id);
  }
  catch (...)
  {
    state = failure;
  }

  if ((exp != none) && (exp != state))
  {
    err() << "error in test: " << suitename << "::" << testname << '\n'
          << "  exited with " << str(state) << ", expected " << str(exp)
          << std::endl;

    exit(1);
  }
}



Crsh::Specimen::Ptr
Crsh::specimen(const std::string& specimen_name)
{
  return byName(db, specimen_name, db->specimen(specimen_name));
}


/** Runs the testcase @ref testcaseId.
 */
void Crsh::runTestcase(TestCaseId testcaseId, expectation expct)
{
  using namespace Rose::BinaryAnalysis;

  typedef Concolic::LinuxExecutor::Result ExecutionResult;
  typedef std::auto_ptr<ExecutionResult>  ExecutionResultGuard;

  Concolic::LinuxExecutorPtr exec     = Concolic::LinuxExecutor::instance();
  Concolic::TestCase::Ptr    testcase = db->object(testcaseId, Concolic::Update::YES);

  ROSE_ASSERT(testcase.getRawPointer());
  err() << "***> " << *testcase << std::endl;

  ExecutionResultGuard result(dynamic_cast<ExecutionResult*>(exec->execute(testcase)));
  expectation          state = result->exitStatus() ? failure : success;

  db->insertConcreteResults(testcase, *result.get());

  if ((expct != none) && (expct != state))
  {
    err() << "error in test: " << testcase->name() << '\n'
          << "  exited with " << str(state) << ", expected " << str(expct)
          << std::endl;

    exit(1);
  }
}


/** Functor to run a new testcase.
 */
struct TestCaseStarter
{
    TestCaseStarter(Crsh& crshobj, Crsh::expectation expct)
    : crsh(crshobj), expect(expct)
    {}

    void operator()(Crsh::TestCaseId id)
    {
      crsh.runTestcase(id, expect);
    }

  private:
    Crsh&             crsh;
    Crsh::expectation expect;
};


void Crsh::run(const char* testsuite, int num, expectation expct)
{
  TestSuite::Ptr suite;

  if (num < 0) num = 1;

  if (testsuite != NULL)
  {
    suite = testSuite(conv(testsuite));
    ROSE_ASSERT(suite);
  }

  db->testSuite(suite);
  std::vector<TestCaseId> tests = db->needConcreteTesting(num);

  std::for_each(tests.begin(), tests.end(), TestCaseStarter(*this, expct));
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
  yyparse();
}

void readEvalPrint(std::istream& input)
{
  static const char  CONTINUE_ON_NEXT_LINE = '\\';
  static const char* EXIT_CMD  = "exit";

  crsh().out() << "crsh v0.0.1\n" << std::endl;

  std::string cmd;

  while (!input.eof() && EXIT_CMD != cmd)
  {
    crsh().out() << ">";
    cmd = CONTINUE_ON_NEXT_LINE;

    // create input from multi-line input
    while (!input.eof() && CONTINUE_ON_NEXT_LINE == lastCh(cmd))
    {
      cmd = cmd.substr(0, cmd.size()-1);
      cmd += rtrim(readLine(input));
    }

    YY_BUFFER_STATE buf = yy_scan_string(cmd.c_str());
    crsh().parse();
    yy_delete_buffer(buf);
  }
}

void parse_file(const char* filename)
{
  yyin = fopen(filename, "r");

  //~ int token;
  //~ while ((token = yylex()) != 0)
      //~ printf("Token: %d (%s)\n", token, yytext);

  crsh().parse();
  fclose(yyin);
}

Crsh& crsh()
{
  static Crsh obj;

  return obj;
}

int main(int argc, char** argv)
{
  if (!TEST_CONCOLICDB)
  {
    std::cerr << "concolic DB testing is disabled." << std::endl;
    return 0;
  }

  if (argc == 1)
  {
    readEvalPrint(std::cin);
  }
  else
  {
    parse_file(argv[1]);
  }

  crsh().disconnect();
  return 0;
}
