#include <vector>
#include <iostream>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <boost/lexical_cast.hpp>


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
  int executeBinary( const std::string& execmon,
                     const std::vector<std::string>& execmonargs,
                     const std::string& binary,
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
  Crsh::annotation_desc notes;
  std::string           specimen;
  Arguments             arguments;

  InvocationDesc(Crsh::annotation_desc usrnotes, const std::string& sp, const Arguments& args)
  : notes(usrnotes), specimen(sp), arguments(args)
  {}
};


//
// Annotation handling

std::string str(int val)
{
  if (val == Crsh::none) return "none";

  std::string res;

  if (val & Crsh::expect_success)   res += "success, ";
  if (val & Crsh::expect_failure)   res += "failure, ";
  if (val & Crsh::reuse_unique)     res += "unique, ";
  if (val & Crsh::reuse_duplicate)  res += "duplicate, ";
  if (val & Crsh::addr_randomize)   res += "ASLR, ";
  if (val & Crsh::addr_norandomize) res += "noASLR, ";

  ROSE_ASSERT(res.size() > 2);
  return res.substr(0, res.size()-2);
}

Crsh::annotation_desc&
operator|=(Crsh::annotation_desc& lhs, Crsh::annotation_desc rhs)
{
  lhs = static_cast<Crsh::annotation_desc>(lhs | rhs);
  return lhs;
}

Crsh::annotation_desc
operator|(Crsh::annotation_desc lhs, Crsh::annotation_desc rhs)
{
  return lhs |= rhs;
}


struct AnnotationHandler
{
    explicit
    AnnotationHandler(std::vector<std::string>& unhandled_notes)
    : unhandled(unhandled_notes), res(Crsh::none)
    {}

    void operator()(const std::string& note)
    {
      if      ("success"      == note) res |= Crsh::expect_success;
      else if ("failure"      == note) res |= Crsh::expect_failure;
      else if ("unique"       == note) res |= Crsh::reuse_unique;
      else if ("duplicate"    == note) res |= Crsh::reuse_duplicate;
      else if ("ASLR"         == note) res |= Crsh::addr_randomize;
      else if ("noASLR"       == note) res |= Crsh::addr_norandomize;
      else                             unhandled.push_back(note);
    }

    operator Crsh::annotation_desc() const { return res; }

  private:
    std::vector<std::string>& unhandled;
    Crsh::annotation_desc     res;
};

Crsh::annotation_desc
convAnnotations(Annotations* annotations, Crsh::annotation_desc enabled, std::vector<std::string>& unhandled)
{
  ASSERT_not_null(annotations);

  return std::for_each( annotations->begin(), annotations->end(), AnnotationHandler(unhandled));
}

Crsh::annotation_desc
convAnnotations(Annotations* annotations, Crsh::annotation_desc enabled = Crsh::all)
{
  std::vector<std::string> unhandled;
  Crsh::annotation_desc    res = convAnnotations(annotations, enabled, unhandled);

  if (unhandled.size())
  {
    std::string err = "unknown annotations: ";

    for (size_t i = 0; i < unhandled.size(); ++i) err += unhandled.at(i);

    throw std::runtime_error(err);
  }

  return res;
}

#ifdef __GNUC__

  static inline
  size_t bitcount32(size_t n)
  {
    return __builtin_popcount(n);
  }

#else /* __GNUC__ */
  #define BITCOUNT32(x) (((BX_(x)+(BX_(x)>>4)) & 0x0F0F0F0F) % 255)
  #define BX_(x) ((x) - (((x)>>1)&0x77777777) - (((x)>>2)&0x33333333) - (((x)>>3)&0x11111111))

  /// \brief  returns the number of bits set in an integer
  /// \param  bnr size_t the integer value
  /// \return the number of bits set in bnr
  static inline
  size_t bitcount32(size_t n)
  {
    return BITCOUNT32(n);
  }

  #undef BITCOUNT32
  #undef BX_
#endif /* __GNUC */


inline
void checkBitcount(unsigned int x)
{
  if (bitcount32(x) > 1)
    throw std::runtime_error("conflicting options");
}


inline
Crsh::annotation_desc subset( Crsh::annotation_desc notes,
                              Crsh::annotation_desc group,
                              Crsh::annotation_desc defaultvalue
                            )
{
  Crsh::annotation_desc res = static_cast<Crsh::annotation_desc>(notes & group);

  if (res == Crsh::none)
    res = defaultvalue;
  else
    checkBitcount(res);

  return res;
}

inline
Crsh::annotation_desc
expect(Crsh::annotation_desc notes)
{
  return subset(notes, Crsh::expect_all, Crsh::expect_default);
}

inline
Crsh::annotation_desc
reuse(Crsh::annotation_desc notes)
{
  return subset(notes, Crsh::reuse_all, Crsh::reuse_default);
}

inline
Crsh::annotation_desc
randomize_address_space(Crsh::annotation_desc notes)
{
  return subset(notes, Crsh::addr_randomize_all, Crsh::addr_randomize_default);
}

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
  const char *envPath = getenv("PATH");
  ASSERT_always_not_null(envPath);
  return findExecutable(p, envPath);
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

void Crsh::closedb()
{
  db = Sawyer::Nothing();
}

void Crsh::connectdb(Annotations* usrnotes, const char* dburl)
{
  std::string url       = conv(dburl);
  annotation_desc exp   = convAnnotations(usrnotes, expect_all);
  int             state = execute_success;

  try
  {
    db = Database::instance(url);
  }
  catch (...)
  {
    state = execute_failure;
  }

  if ((expect(exp) != none) && (expect(exp) != state))
  {
    err() << "error when connecting to database: " << url << '\n'
          << "  exited with " << str(state) << ", expected " << str(exp)
          << std::endl;

    throw std::runtime_error("error connecting to DB");
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

char* Crsh::unquoteString(const char* str)
{
  size_t len   = strlen(str)-1;
  ROSE_ASSERT(len > 0 && str[0] == '\"' && str[len] == '\"');

  char*  clone = static_cast<char*>(malloc(len));
  ASSERT_always_not_null(clone);
  strncpy(clone, str+1, len-1);
  clone[len-1] = 0;

  free(const_cast<char*>(str));
  return clone;
}

std::string*
Crsh::arg(const char* argument) const
{
  std::string tmp = conv(argument);

  return new std::string(tmp);
}

std::string*
Crsh::arg(int argument) const
{
  return new std::string(boost::lexical_cast<std::string>(argument));
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


std::string*
Crsh::annotation(const char* argument) const
{
  std::string tmp = conv(argument);

  return new std::string(tmp);
}


Annotations*
Crsh::annotations() const
{
  return new Annotations();
}

Annotations*
Crsh::annotations(Annotations* annotations, const std::string* note) const
{
  return enlist(annotations, note);
}


InvocationDesc*
Crsh::invoke(Annotations* notes, const char* specimen, Arguments* args) const
{
  std::auto_ptr<Arguments> arguments(args);
  std::string              specimenname = conv(specimen);
  annotation_desc          annotations  = convAnnotations(notes, reuse_all);

  specimenname = findExecutable(bstfs::path(specimenname)).native();
  return new InvocationDesc(annotations, specimenname, *arguments.get());
}

void
Crsh::setMonitor(const char* str)
{
  execmon = conv(str);
}

template <class T>
static inline
std::vector<T> mkVector(const std::list<T>& lst)
{
  return std::vector<T>(lst.begin(), lst.end());
}


void
Crsh::test(const char* ts, const char* tst, Annotations* usrnotes, Environment* env, InvocationDesc* inv)
{
  std::auto_ptr<Environment>    envguard(env);
  std::auto_ptr<InvocationDesc> invguard(inv);
  std::string                   suitename = conv(ts);
  std::string                   testname  = conv(tst);
  annotation_desc               expct     = convAnnotations(usrnotes, expect_all);
  int                           state     = execute_success;
  std::string                   failureMessage;

  try
  {
    // get the object for the specimen's name
    Specimen::Ptr               specObj   = specimen(inv->specimen, reuse(inv->notes));
    TestCase::Ptr               testObj   = TestCase::instance(specObj);

    testObj->name(testname);
    testObj->args(mkVector(inv->arguments));
    testObj->env(mkVector(*env));

    TestCaseId                  testId    = db->id(testObj);
    TestSuite::Ptr              suiteObj  = testSuite(suitename);
    if (!suiteObj) {
        // if the test suite doesn't exist then create it. [Robb Matzke 2019-08-14]
        suiteObj = TestSuite::instance(suitename);
    }
    ASSERT_not_null(suiteObj);
    TestSuiteId                 suiteId  = db->id(suiteObj);

    ROSE_ASSERT(testId);
    ROSE_ASSERT(suiteId);

    db->assocTestCaseWithTestSuite(testId, suiteId);
  }
  catch (const Rose::BinaryAnalysis::Concolic::Exception &e) {
    failureMessage = e.what();
    state = execute_failure;
  }
  catch (...) {
    state = execute_failure;
  }

  if ((expct != none) && (expct != state))
  {
    err() << "error in test: " << suitename << "::" << testname << '\n'
          << "  exited with " << str(state) << ", expected " << str(expct)
          << std::endl;

    if (state == execute_failure)
        err() <<"  failure was: " <<(failureMessage.empty() ? "unknown" : failureMessage) <<"\n";

    throw std::runtime_error("failed test");
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
Crsh::specimen(const std::string& specimen_name, annotation_desc reuse)
{
  if (reuse_duplicate == reuse)
  {
    Specimen::Ptr specimen = Specimen::instance(specimen_name);

    db->id(specimen); // \todo \pp
                      //   storing the object may be not necessary
                      //   b/c it will be stored with the test case anyways.
    return specimen;
  }

  ROSE_ASSERT(reuse_unique == reuse);
  std::vector<SpecimenId> found = db->findSpecimensByName(specimen_name);

  if (found.empty())
  {
    Specimen::Ptr specimen = Specimen::instance(specimen_name);
    SpecimenId    specId = db->id(specimen); // \todo \pp see note above

    return specimen;
  }

  if (found.size() > 1)
    throw std::runtime_error("specimen name is not unique");

  return db->object(found.at(0));
}


/** Runs the testcase @ref testcaseId.
 */
void Crsh::runTestcase(TestCaseId testcaseId, annotation_desc allnotes)
{
  using namespace Rose::BinaryAnalysis;

  typedef Concolic::LinuxExecutor::Result ExecutionResult;
  typedef std::auto_ptr<ExecutionResult>  ExecutionResultGuard;

  int                        state    = execute_failure;
  int                        processDisposition = 0; // disposition returned by waitpid
  Concolic::LinuxExecutorPtr exec     = Concolic::LinuxExecutor::instance();
  Concolic::TestCase::Ptr    testcase = db->object(testcaseId, Concolic::Update::YES);
  annotation_desc            expct    = expect(allnotes);
  const bool                 addrRandomize = (randomize_address_space(allnotes) == addr_randomize);

  exec->useAddressRandomization(addrRandomize);
  exec->executionMonitor(execmon);

  if (testcase.getRawPointer())
  {
    ExecutionResultGuard result(exec->execute(testcase));

    processDisposition = result->exitStatus();
    db->insertConcreteResults(testcase, *result.get());

    // exitValue is not the argument to the test's "exit" function, but rather the process disposition returned by
    // waitpid. Therefore, success should be measured as the test having normal termination with an exit status of zero.
    if (WIFEXITED(processDisposition) && WEXITSTATUS(processDisposition) == 0)
        state = execute_success;
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

    throw std::runtime_error("failed test");
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
  std::string                   withoutExecMonitor;
  std::vector<std::string>      monitorArgs;
  std::vector<std::string>      args = mkVector(invoc->arguments);
  std::vector<std::string>      envv = currentEnvironment();

  int ec = Concolic::executeBinary( withoutExecMonitor,
                                    monitorArgs,
                                    invoc->specimen,
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
    TestCaseStarter(Crsh& crshobj, Crsh::annotation_desc usrnotes)
    : crsh(crshobj), notes(usrnotes)
    {}

    void operator()(Crsh::TestCaseId id)
    {
      crsh.runTestcase(id, notes);
    }

  private:
    Crsh&                 crsh;
    Crsh::annotation_desc notes;
};

void
Crsh::runTest(Annotations* usrnotes, const char* testsuite, int num)
{
  TestSuite::Ptr  suite;
  annotation_desc allnotes = convAnnotations(usrnotes, expect_all | addr_randomize_all);
  annotation_desc expct    = expect(allnotes);

  if (num < 0) num = 1;

  if (testsuite != NULL)
  {
    std::string tsname = conv(testsuite);

    suite = testSuite(tsname, false /* do not create new test suites */);

    if (!suite.getRawPointer())
    {
      if (expct == expect_failure) return;

      err() << "unable to find testsuite: " << tsname << '\n'
            << "  exited with " << str(execute_failure) << ", expected " << str(expct)
            << std::endl;
      throw std::runtime_error("unable to find testsuite " + tsname);
    }
  }

  db->testSuite(suite);
  std::vector<TestCaseId> tests = db->needConcreteTesting(num);

  std::for_each(tests.begin(), tests.end(), TestCaseStarter(*this, allnotes));
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
  ASSERT_always_not_null(yyin);

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
