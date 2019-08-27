#include <vector>
#include <iostream>
#include <csignal>
#include <cstdio>
#include <cstdlib>

#if defined(__linux__)
  #include <unistd.h>
#endif

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

namespace bstfs = boost::filesystem;

// stealing from LinuxExecutor.C
namespace Rose { namespace BinaryAnalysis { namespace Concolic {
  int executeBinary( const std::string& binary,
                     const std::string& logout,
                     const std::string& logerr,
                     LinuxExecutor::Persona persona,
                     std::vector<std::string> args,
                     std::vector<std::string> envv
                   );
}}}

//
// InvocationDesc

//! InvocationDesc is opaque for other translation units

struct InvocationDesc
{
  Crsh::specimenreuse reusenote;
  std::string         specimen;
  Arguments           arguments;

  InvocationDesc(Crsh::specimenreuse reuse, const std::string& sp, const Arguments& args)
  : reusenote(reuse), specimen(sp), arguments(args)
  {}
};

//
// auxiliary functions

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
  ASSERT_not_null(str);

  std::string res(str);

  free(const_cast<char*>(str));
  return res;
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

bool isExecutable(const bstfs::path& p)
{
#if defined(__linux__)
  return access(p.c_str(), X_OK) == 0;
#else
  throw std::runtime_error("Non-Linux system");
#endif /* __linux__ */
}

void validateExistance(const bstfs::path& p)
{
  if (!bstfs::exists(p))
    std::runtime_error("executable does not exist: " + p.native());
}

void validateExecutability(const bstfs::path& p)
{
  if (!isExecutable(p))
    std::runtime_error("cannot execute file: " + p.native());
}


bstfs::path
findExecutable(const bstfs::path& exe, std::string searchpath)
{
  while (searchpath.size())
  {
    const size_t      sep  = searchpath.find_first_of(':');
    const std::string cand = searchpath.substr(0, sep);
    const bstfs::path full(cand + "/" + exe.native());

    if (bstfs::exists(full) && isExecutable(full))
      return full;

    const size_t      remain = (sep == std::string::npos) ? searchpath.size()
                                                          : sep+1
                                                          ;

    searchpath = searchpath.substr(remain);
  }

  // throw std::runtime_error("executable not found in path: " + exe.native());
  return exe;
}


bstfs::path
findExecutable(bstfs::path p)
{
  // if path is specified
  if (p.parent_path() != "")
  {
    validateExistance(p);
    validateExecutability(p);

    return p;
  }

  // finds executable in path
  return findExecutable(p, getenv("PATH"));
}

std::string
findExecutable(const std::string& p)
{
  // finds executable in path
  return findExecutable(p);
}

#if defined(__linux__)
extern char **environ; /**< link to my environment. */
#endif

std::vector<std::string>
currentEnvironment()
{
  std::vector<std::string> res;

#if defined(__linux__)
  char** envvar = environ;

  while (*envvar)
  {
    res.push_back(*envvar);

    ++envvar;
  }
#endif

  return res;
}


//
// Crsh implementation

Crsh::expectation
expectationNote(const char* expect)
{
  if (!expect) return Crsh::none;

  std::string note = conv(expect);

  if ("success" == note) return Crsh::success;
  if ("failure" == note) return Crsh::failure;

  std::cerr << "unexpected expectation note: " << note << std::endl;
  exit(1);
}

Crsh::specimenreuse
reuseNote(const char* reusenote)
{
  if (!reusenote) return Crsh::defaultreuse;

  std::string note = conv(reusenote);

  if ("unique"    == note) return Crsh::unique;
  if ("duplicate" == note) return Crsh::duplicate;

  std::cerr << "unexpected reuse note: " << note << std::endl;
  exit(1);
}


void Crsh::closedb()
{
  db = Sawyer::Nothing();
}

void Crsh::connectdb(const char* dburl, const char* expect)
{
  std::string url   = conv(dburl);
  expectation exp   = expectationNote(expect);
  expectation state = success;

  try
  {
    db = Database::instance(url);
  }
  catch (...)
  {
    state = failure;
  }

  if ((exp != none) && (exp != state))
  {
    err() << "error when connecting to database: " << url << '\n'
          << "  exited with " << str(state) << ", expected " << str(exp)
          << std::endl;

    exit(1);
  }
}

void Crsh::createdb(const char* dburl)
{
  db = Database::create(conv(dburl));
}

void Crsh::createdb(const char* dburl, const char* testsuite)
{
  db = Database::create(conv(dburl), conv(testsuite));

  // for now just clear the testsuite
  db->testSuite(TestSuite::Ptr());
}

Crsh::TestSuite::Ptr
Crsh::testSuite(const std::string& s, bool createMissingEntry)
{
    return db->findTestSuite(s);
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

/*
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
*/

char* Crsh::unquoteString(const char* str)
{
  size_t len   = strlen(str)-1;
  ROSE_ASSERT(len > 0 && str[0] == '\"' && str[len] == '\"');

  char*  clone = static_cast<char*>(malloc(len));
  strncpy(clone, str+1, len-1);
  clone[len-1] = 0;

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
Crsh::invoke(const char* note, const char* specimen, Arguments* args) const
{
  Crsh::specimenreuse      reuse        = reuseNote(note);
  std::string              specimenname = conv(specimen);
  std::auto_ptr<Arguments> arguments(args);

  specimenname = findExecutable(bstfs::path(specimenname)).native();
  return new InvocationDesc(reuse, specimenname, *args);
}

template <class T>
static inline
std::vector<T> mkVector(const std::list<T>& lst)
{
  return std::vector<T>(lst.begin(), lst.end());
}


void
Crsh::test(const char* ts, const char* tst, const char* expct, Environment* env, InvocationDesc* inv)
{
  std::string                   suitename = conv(ts);
  std::string                   testname  = conv(tst);
  expectation                   exp       = expectationNote(expct);
  std::auto_ptr<Environment>    envguard(env);
  std::auto_ptr<InvocationDesc> invguard(inv);
  expectation                   state     = success;
  std::string                   failureMessage;

  try
  {
    // get the object for the specimen's name
    Specimen::Ptr               specobj = specimen(inv->specimen, inv->reusenote);
    TestCase::Ptr               test    = TestCase::instance(specobj);

    test->name(testname);
    test->args(mkVector(inv->arguments));
    test->env(mkVector(*env));

    TestCaseId                  id        = db->id(test);
    TestSuite::Ptr              suite_obj = testSuite(suitename);
    if (!suite_obj) {
        // if the test suite doesn't exist then create it. [Robb Matzke 2019-08-14]
        suite_obj = TestSuite::instance(suitename);
    }
    ASSERT_not_null(suite_obj);
    TestSuiteId                 suite_id  = db->id(suite_obj);

    ROSE_ASSERT(id);
    ROSE_ASSERT(suite_id);

    db->assocTestCaseWithTestSuite(id, suite_id);
  }
  catch (const Rose::BinaryAnalysis::Concolic::Exception &e) {
    failureMessage = e.what();
    state = failure;
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
    if (state == failure)
        err() <<"  failure was: " <<(failureMessage.empty() ? "unknown" : failureMessage) <<"\n";

    exit(1);
  }
}


template <class T>
static
typename T::Ptr
byName( Crsh::Database::Ptr& db,
        const std::string& s,
        Rose::BinaryAnalysis::Concolic::ObjectId<T> id,
        bool createMissingEntry = true
      )
{
  if (id) return db->object(id);

  if (!createMissingEntry) return typename T::Ptr();

  // if not in the database already, create it from a file
  typename T::Ptr obj = T::instance(s);

  db->id(obj);
  return obj;
}



Crsh::Specimen::Ptr
Crsh::specimen(const std::string& specimen_name, specimenreuse reuse)
{
    if (duplicate == reuse)
    {
      Specimen::Ptr specimen = Specimen::instance(specimen_name);

      db->id(specimen);
      return specimen;
    }

    ROSE_ASSERT(unique == reuse);
    std::vector<SpecimenId> found = db->findSpecimensByName(specimen_name);

    if (found.empty())
    {
      Specimen::Ptr specimen = Specimen::instance(specimen_name);

      db->id(specimen);
      return specimen;
    }

    if (found.size() > 1)
      throw std::runtime_error("specimen name is not unique");

    return db->object(found.at(0));
}


/** Runs the testcase @ref testcaseId.
 */
void Crsh::runTestcase(TestCaseId testcaseId, expectation expct)
{
  using namespace Rose::BinaryAnalysis;

  typedef Concolic::LinuxExecutor::Result ExecutionResult;
  typedef std::auto_ptr<ExecutionResult>  ExecutionResultGuard;

  expectation                state    = failure;
  int                        processDisposition = 0; // disposition returned by waitpid
  Concolic::LinuxExecutorPtr exec     = Concolic::LinuxExecutor::instance();
  Concolic::TestCase::Ptr    testcase = db->object(testcaseId, Concolic::Update::YES);

  if (testcase.getRawPointer())
  {
    ExecutionResultGuard result(dynamic_cast<ExecutionResult*>(exec->execute(testcase)));

    processDisposition = result->exitStatus();
    db->insertConcreteResults(testcase, *result.get());

    // exitValue is not the argument to the test's "exit" function, but rather the process disposition returned by
    // waitpid. Therefore, success should be measured as the test having normal termination with an exit status of zero.
    if (WIFEXITED(processDisposition) && WEXITSTATUS(processDisposition) == 0)
        state = success;
  }

  if ((expct != none) && (expct != state))
  {
    err() << "error in runTestcase: " << testcase->name() << '\n'
          << "  exited with " << str(state) << ", expected " << str(expct) << '\n'
          << "  process disposition: ";
    if (WIFEXITED(processDisposition)) {
        err() <<"exit value " <<WEXITSTATUS(processDisposition) <<"\n";
    } else if (WIFSIGNALED(processDisposition)) {
        err() <<"death by signal " <<strsignal(WTERMSIG(processDisposition)) <<"\n";
    } else if (WIFSTOPPED(processDisposition)) {
        err() <<"process stopped by " <<strsignal(WSTOPSIG(processDisposition)) <<"\n";
    } else if (WIFCONTINUED(processDisposition)) {
        err() <<"process resumed\n";
    }

    exit(1);
  }
}


// immediately invokes the described program
void Crsh::execute(InvocationDesc* invoc)
{
  using namespace Rose::BinaryAnalysis;

  typedef Concolic::LinuxExecutor::Persona Persona;

  std::auto_ptr<InvocationDesc> invguard(invoc);
  const std::string             noredirect;
  Persona                       nopersona;
  std::vector<std::string>      args = mkVector(invoc->arguments);
  std::vector<std::string>      envv = currentEnvironment();

  int ec = Concolic::executeBinary( invoc->specimen,
                                    noredirect,
                                    noredirect,
                                    nopersona,
                                    args,
                                    envv
                                  );

  if (ec) err() << "error: " << invoc->specimen << " exited with " << ec << std::endl;
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


void Crsh::runTest(const char* testsuite, int num, const char* expect)
{
  TestSuite::Ptr suite;
  expectation    expct = expectationNote(expect);

  if (num < 0) num = 1;

  if (testsuite != NULL)
  {
    std::string tsname = conv(testsuite);

    suite = testSuite(tsname, false /* do not create new test suites */);

    if (!suite.getRawPointer())
    {
      if (expct == failure) return;

      err() << "unable to find testsuite: " << tsname << '\n'
            << "  exited with " << str(failure) << ", expected " << str(expct)
            << std::endl;
    }
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

  crsh().closedb();
  return 0;
}
