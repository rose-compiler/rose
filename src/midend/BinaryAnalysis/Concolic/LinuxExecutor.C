#include <sage3basic.h>

#if 0 /* __cplusplus >= 201103L */
#include <boost/process.hpp>
#else
#include <sys/wait.h>
#include <sys/personality.h>
#endif

#include <boost/atomic.hpp>
#include <boost/lexical_cast.hpp>
#include <BinaryConcolic.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
  
typedef Sawyer::Optional<unsigned long> Persona;

LinuxExecutor::Result::Result(int exitStatus)
    : ConcreteExecutor::Result(0.0), exitStatus_(exitStatus) {
    // FIXME[Robb Matzke 2019-04-15]: probably want a better ranking that 0.0, such as a ranking that depends on the exit status.
}

char* c_str_ptr(std::string& s)
{
  return const_cast<char*>(s.c_str());
}

std::string to_std_string(const EnvValue& v)
{
  return v.first + v.second;
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

  if (pid)
  {
    int status = 0;

    waitpid(pid, &status, 0); // wait for the child to exit
    return status;
  }
  
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
  
  envv.reserve(1 /* delimter */ +env_strings.size());  
  std::transform(env_strings.begin(), env_strings.end(), std::back_inserter(envv), c_str_ptr);
  envv.push_back(NULL);
  
  
  
  // execute the program
  /* const int err = */ execvpe(args[0], &args[0], &envv[0]);
  exit(0);
}
#endif /* after boost 1.65 and C++11 */

/*
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
*/

//~ static atomic_counter<int> versioning(0);
static boost::atomic<int> versioning(0);

ConcreteExecutor::Result*
LinuxExecutor::execute(const TestCase::Ptr& tc)
{
  typedef ConcreteExecutor::Result* ResultType;

  namespace bstfs = boost::filesystem;

  int         uniqNum  = versioning.fetch_add(1);
  std::string basename = "./out";

  basename.append(boost::lexical_cast<std::string>(uniqNum));

  bstfs::path binary(basename + ".bin");
  bstfs::path logout(basename + "_out.log");
  bstfs::path logerr(basename + "_err.log");

  storeBinaryFile(tc->specimen()->content(), binary);
  bstfs::permissions(binary, bstfs::owner_exe);
  
  Persona persona;
  
  if (useAddressRandomization_) persona = Persona(ADDR_NO_RANDOMIZE);
  
  const int   errcode = execute_binary(binary, logout, logerr, persona, tc);

  // cleanup
  bstfs::remove(logerr);
  bstfs::remove(logout);
  bstfs::remove(binary);

  return ResultType(new LinuxExecutor::Result(errcode));
}

} // namespace
} // namespace
} // namespace
