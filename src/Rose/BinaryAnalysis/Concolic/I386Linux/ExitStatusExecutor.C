#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/I386Linux/ExitStatusExecutor.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/I386Linux/ExitStatusResult.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/FileSystem.h>

#include <boost/lexical_cast.hpp>
#include <fcntl.h>
#include <sys/personality.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#if BOOST_VERSION >= 105300
#include <boost/atomic.hpp>
#endif /* BOOST_VERSION */

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

namespace
{
  char* c_str_ptr(const std::string& s)
  {
    return const_cast<char*>(s.c_str());
  }

  std::string to_std_string(const EnvValue& v)
  {
    return v.first + "=" + v.second;
  }
}


std::vector<std::string>
convToStringVector(std::vector<EnvValue> env)
{
  std::vector<std::string> res;

  res.reserve(env.size());
  std::transform(env.begin(), env.end(), std::back_inserter(res), to_std_string);

  return res;
}

#if 0 /* after boost 1.65 and C++11 */
// \todo update interface (see below)
int executeBinary(  const boost::filesystem::path& binary,
                    const boost::filesystem::path& logout,
                    const boost::filesystem::path& logerr,
                    TestCase::Ptr tc
                  )
{
  namespace bstpc = boost::process;

  bstpc::child    sub(binary, bstpc::std_out > logout, bstpc::std_err > logerr);
  std::error_code ec;

  sub.wait(ec);
  return ec.value();
}
#else

void redirectStream(const std::string& ofile, int num)
{
  if (ofile.size() == 0) return;

  // FIXME[Robb Matzke 2020-01-15]: possible bug. open returns -1 on failure, not zero. Should you
  // close outstream before returning?
  int outstream = open(ofile.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  if (outstream) dup2(outstream, num);
}

void setPersonality(ExitStatusExecutor::Persona persona)
{
    if (persona) {
        if (-1 == personality(persona.get()))
            throw Exception("cannot set personality");
    }
}

// Returns the exit status as documented by waitpid[2], which is not the same as the argument to the child's exit[3] call.
int executeBinary( const std::string& execmon,
                   const std::vector<std::string>& execmonargs,
                   const std::string& binary,
                   const std::string& logout,
                   const std::string& logerr,
                   ExitStatusExecutor::Persona persona,
                   std::vector<std::string> arguments,
                   std::vector<std::string> environment
                 )
{
  int pid = fork();

  if (pid < 0) throw std::runtime_error("unable to fork process.");

  if (pid)
  {
    // parent process
    int status = 0;

    waitpid(pid, &status, 0); // wait for the child to exit
    return status;
  }

  // child process
  redirectStream(logout, STDOUT_FILENO);
  redirectStream(logerr, STDERR_FILENO);
  setPersonality(persona);

  std::vector<char*>       args;  // points to arguments
  std::vector<char*>       envv;  // points to environment strings
  const bool               withExecMonitor = execmon.size() > 0;

  args.reserve(2 /* program name + delimiter */ + arguments.size() + execmonargs.size());

  if (withExecMonitor)
  {
    std::transform(execmonargs.begin(), execmonargs.end(), std::back_inserter(args), c_str_ptr);
  }

  // set up arguments
  args.push_back(const_cast<char*>(binary.c_str()));
  std::transform(arguments.begin(), arguments.end(), std::back_inserter(args), c_str_ptr);
  args.push_back(NULL);

  // set up env
  envv.reserve(1 /* delimiter */ + environment.size());
  std::transform(environment.begin(), environment.end(), std::back_inserter(envv), c_str_ptr);
  envv.push_back(NULL);

  // execute the program
  const int errc = execvpe(args[0], &args[0], &envv[0]);
  ASSERT_always_require(-1 == errc);

  perror("exec failed");
  exit(EXIT_FAILURE);
}


int executeBinary( const boost::filesystem::path&  execmon,
                   const std::vector<std::string>& execmonargs,
                   const boost::filesystem::path&  binary,
                   const boost::filesystem::path&  logout,
                   const boost::filesystem::path&  logerr,
                   ExitStatusExecutor::Persona persona,
                   TestCase::Ptr                   tc
                 )
{
  return executeBinary( execmon.string(),
                        execmonargs,
                        binary.string(),
                        logout.string(),
                        logerr.string(),
                        persona,
                        tc->args(),
                        convToStringVector(tc->env())
                      );
}
#endif /* after boost 1.65 and C++11 */

#if BOOST_VERSION >= 105300
typedef boost::atomic<int> atomic_counter_t;
#else

// when boost does not have atomic
template <class T>
struct atomic_counter
{
  explicit
  atomic_counter(T init)
  : val(init), mutex_()
  {}

  T fetch_add(T incr)
  {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

    T res = val;

    val += incr;
    return res;
  }

  private:
    T val;
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;
};

typedef atomic_counter<int> atomic_counter_t;

#endif /* BOOST_VERSION */

//~ static atomic_counter<int> versioning(0);
static atomic_counter_t versioning(0);

ExitStatusResult::Ptr
createLinuxResult(int errcode, std::string outstr, std::string errstr, double rank) {
    auto res = ExitStatusResult::instance(rank, errcode);
    res->out(outstr);
    res->err(errstr);
    return res;
}

ExitStatusExecutor::ExitStatusExecutor(const std::string &name)
    : Super(name) {}

ExitStatusExecutor::ExitStatusExecutor(const Database::Ptr &db)
    : Super(db) {}

ExitStatusExecutor::~ExitStatusExecutor() {}

ExitStatusExecutor::Ptr
ExitStatusExecutor::instance(const Database::Ptr &db) {
    return Ptr(new ExitStatusExecutor(db));
}

ExitStatusExecutor::Ptr
ExitStatusExecutor::factory() {
    return Ptr(new ExitStatusExecutor("I386Linux::ExitStatus"));
}

Concolic::ConcreteExecutor::Ptr
ExitStatusExecutor::instanceFromFactory(const Database::Ptr &db) {
    ASSERT_require(isFactory());
    auto retval = instance(db);
    retval->name(name());
    return retval;
}

bool
ExitStatusExecutor::matchFactory(const std::string &name) const {
    return name == this->name();
}

Concolic::ConcreteResult::Ptr
ExitStatusExecutor::execute(const TestCase::Ptr& tc) {
  ASSERT_forbid(isFactory());
  namespace bstfs = boost::filesystem;

  const bool               withExecMonitor = executionMonitor().string().size();
  int                      uniqNum  = versioning.fetch_add(1);
  int                      procNum  = getpid();
  std::string              basename = "./out_";
  SpecimenPtr              specimen = tc->specimen();

  basename.append(boost::lexical_cast<std::string>(procNum));
  basename.append("_");
  basename.append(boost::lexical_cast<std::string>(uniqNum));

  bstfs::path              binary(basename + ".bin");
  bstfs::path              logout(basename + "_out.log");
  bstfs::path              logerr(basename + "_err.log");
  bstfs::path              qualScore(basename + ".qs");

  FileSystem::writeFile(binary, specimen->content());
  bstfs::permissions(binary, bstfs::add_perms | bstfs::owner_read | bstfs::owner_exe);

  Persona                  persona;
  std::vector<std::string> execmonArgs;

  if (!useAddressRandomization_) persona = Persona(ADDR_NO_RANDOMIZE);

  if (withExecMonitor)
  {
    // execution monitor was set
    execmonArgs.reserve(5);

    execmonArgs.push_back(executionMonitor().string());
    execmonArgs.push_back("-o");
    execmonArgs.push_back(qualScore.string());
    // execmonArgs.push_back("--no-disassembler");
  }

  int                      errcode = executeBinary( executionMonitor(),
                                                    execmonArgs,
                                                    binary,
                                                    logout,
                                                    logerr,
                                                    persona,
                                                    tc
                                                  );

  const std::string        outstr  = FileSystem::readFile<std::string>(logout);
  const std::string        errstr  = FileSystem::readFile<std::string>(logerr);
  double                   rank    = errcode;

  if (withExecMonitor)
  {
    std::stringstream results(FileSystem::readFile<std::string>(qualScore));
    results >> errcode >> rank;
    bstfs::remove(qualScore);
  }

  // cleanup
  bstfs::remove(logerr);
  bstfs::remove(logout);
  bstfs::remove(binary);

  tc->concreteRank(rank);
  return createLinuxResult(errcode, outstr, errstr, rank);
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
