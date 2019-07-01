#include <sage3basic.h>

#if 0 /* __cplusplus >= 201103L */
#include <boost/process.hpp>
#elif defined(__linux__)
#include <sys/wait.h>
#include <sys/personality.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#else
// nothing
#endif

#include <BinaryConcolic.h>

#include <boost/lexical_cast.hpp>

#if BOOST_VERSION >= 105300
#include <boost/atomic.hpp>
#endif /* BOOST_VERSION */

#include "io-utility.h"

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Concolic::ConcreteExecutor::Result)
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Concolic::LinuxExecutor::Result)
#endif /* ROSE_HAVE_BOOST_SERIALIZATION_LIB */

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

#if defined(__linux__)

/*****
 **  LinuxExecutors work best on Linux
 *****/


namespace
{
  char* c_str_ptr(std::string& s)
  {
    return const_cast<char*>(s.c_str());
  }

  std::string to_std_string(const EnvValue& v)
  {
    return v.first + "=" + v.second;
  }
}


typedef Sawyer::Optional<unsigned long> Persona;

LinuxExecutor::Result::Result(int exitStatus)
    : ConcreteExecutor::Result(0.0), exitStatus_(exitStatus) {
    // FIXME[Robb Matzke 2019-04-15]: probably want a better ranking that 0.0, such as a ranking that depends on the exit status.
}

std::vector<std::string>
conv_to_string_vector(std::vector<EnvValue> env)
{
  std::vector<std::string>  res;

  res.reserve(env.size());
  std::transform(env.begin(), env.end(), std::back_inserter(res), to_std_string);

  return res;
}

#if 0 /* after boost 1.65 and C++11 */
int execute_binary( const boost::filesystem::path& binary,
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
int execute_binary( const boost::filesystem::path& binary,
                    const boost::filesystem::path& logout,
                    const boost::filesystem::path& logerr,
                    Persona persona,
                    TestCase::Ptr tc
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
  int outstream = open(logout.string().c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  int errstream = open(logerr.string().c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  dup2(outstream, STDOUT_FILENO);
  dup2(errstream, STDERR_FILENO);

  if (persona) personality(persona.get());

  std::string              tc_binary    = binary.string();
  std::vector<std::string> tc_arguments = tc->args(); // holds arguments
  std::vector<char*>       args;  // points to arguments

  // set up arguments
  args.reserve(2 /* program name + delimiter */ + tc_arguments.size());
  args.push_back(const_cast<char*>(tc_binary.c_str()));
  std::transform(tc_arguments.begin(), tc_arguments.end(), std::back_inserter(args), c_str_ptr);
  args.push_back(NULL);

  std::vector<std::string> env_strings = conv_to_string_vector(tc->env()); // holds environment strings
  std::vector<char*>       envv;        // points to environment strings

  envv.reserve(1 /* delimiter */ + env_strings.size());
  std::transform(env_strings.begin(), env_strings.end(), std::back_inserter(envv), c_str_ptr);
  envv.push_back(NULL);

  // execute the program
  /* const int err = */ execvpe(args[0], &args[0], &envv[0]);
  exit(0);
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


LinuxExecutor::Result*
createLinuxResult(int errcode, std::string outstr, std::string errstr)
{
  LinuxExecutor::Result* res = new LinuxExecutor::Result(errcode);

  res->exitStatus(errcode);
  res->out(outstr);
  res->err(errstr);
  return res;
}

ConcreteExecutor::Result*
LinuxExecutor::execute(const TestCase::Ptr& tc)
{
  namespace bstfs = boost::filesystem;

  int         uniqNum  = versioning.fetch_add(1);
  int         procNum  = getpid();
  std::string basename = "./out_";
  SpecimenPtr specimen = tc->specimen();

  basename.append(boost::lexical_cast<std::string>(procNum));
  basename.append("_");
  basename.append(boost::lexical_cast<std::string>(uniqNum));

  bstfs::path binary(basename + ".bin");
  bstfs::path logout(basename + "_out.log");
  bstfs::path logerr(basename + "_err.log");

  storeBinaryFile(specimen->content(), binary);

#if BOOST_VERSION >= 105300
  bstfs::permissions(binary, bstfs::owner_exe);
#else
  ROSE_ASSERT(false);
#endif /* BOOST_VERSION */

  Persona persona;

  if (!useAddressRandomization_) persona = Persona(ADDR_NO_RANDOMIZE);

  const int   errcode = execute_binary(binary, logout, logerr, persona, tc);
  std::string outstr  = loadTextFile(logout);
  std::string errstr  = loadTextFile(logerr);

  // cleanup
  bstfs::remove(logerr);
  bstfs::remove(logout);
  bstfs::remove(binary);

  return createLinuxResult(errcode, outstr, errstr);
}

#else // !defined (__linux__)

LinuxExecutor::Result::Result(int exitStatus)
    : ConcreteExecutor::Result(0.0), exitStatus_(exitStatus) {
    ROSE_ASSERT(!"NOT_LINUX");
}

ConcreteExecutor::Result*
LinuxExecutor::execute(const TestCase::Ptr& tc)
{
  ROSE_ASSERT(!"NOT_LINUX");
}

#endif

} // namespace
} // namespace
} // namespace
