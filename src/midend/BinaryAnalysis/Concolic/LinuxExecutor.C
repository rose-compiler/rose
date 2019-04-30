
#if 0 /* __cplusplus >= 201103L */
#include <boost/process.hpp>
#else
#include <sys/wait.h>
#endif

#include <boost/atomic.hpp>
#include <boost/lexical_cast.hpp>
#include <sage3basic.h>
#include <BinaryConcolic.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

LinuxExecutor::Result::Result(int exitStatus)
    : ConcreteExecutor::Result(0.0), exitStatus_(exitStatus) {
    // FIXME[Robb Matzke 2019-04-15]: probably want a better ranking that 0.0, such as a ranking that depends on the exit status.
}



// https://stackoverflow.com/questions/31131907/writing-into-binary-file-with-the-stdostream-iterator
template <class T, class CharT = char, class Traits = std::char_traits<CharT> >
struct ostreambin_iterator : std::iterator<std::output_iterator_tag, void, void, void, void>
{
  typedef std::basic_ostream<CharT, Traits> ostream_type;
  typedef Traits                            traits_type;
  typedef CharT                             char_type;

  ostreambin_iterator(ostream_type& s) : stream(s) { }

  ostreambin_iterator& operator=(const T& value)
  {
    // basic implementation for demonstration
    stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
    return *this;
  }

  ostreambin_iterator& operator*()     { return *this; }
  ostreambin_iterator& operator++()    { return *this; }
  ostreambin_iterator& operator++(int) { return *this; }

  ostream_type& stream;
};

template <class T>
struct FileSink
{
  typedef ostreambin_iterator<T> insert_iterator;

  std::ostream& datastream;

  FileSink(std::ostream& stream)
  : datastream(stream)
  {}

  void reserve(size_t) {}

  insert_iterator
  inserter()
  {
    return insert_iterator(datastream);
  }
};


void
place_binary(Specimen::Ptr specimen, const boost::filesystem::path& binary)
{
  std::ofstream outfile(binary.string(), std::ofstream::binary);

  assert(outfile.good());

  const std::vector<uint8_t>& executable = specimen->content();
  FileSink<char>              sink(outfile);

  sink.reserve(executable.size());
  std::copy(executable.begin(), executable.end(), sink.inserter());
}

char* as_c_str(std::string& s)
{
  return const_cast<char*>(s.c_str());
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

  std::string              tc_binary    = binary.string();
  std::vector<std::string> tc_arguments = tc->args(); 
  std::vector<char*>       args;
  
  // set up arguments
  args.reserve(2 /* program name + delimiter */ + tc_arguments.size());
  args.push_back(const_cast<char*>(tc_binary.c_str()));
  std::transform(tc_arguments.begin(), tc_arguments.end(), std::back_inserter(args), as_c_str);
  args.push_back(NULL);
  
  // execute the code
  const int   err = execvp(args[0], &args[0]);

  return err;
}
#endif /* after boost 1.65 and C++11 */

static boost::atomic<int> versioning(0);

boost::movelib::unique_ptr<ConcreteExecutor::Result>
LinuxExecutor::execute(const TestCase::Ptr& tc)
{
  typedef boost::movelib::unique_ptr<ConcreteExecutor::Result> ResultType;

  namespace bstfs = boost::filesystem;

  int         uniqNum  = versioning.fetch_add(1);
  std::string basename = "./out";

  basename.append(boost::lexical_cast<std::string>(uniqNum));

  bstfs::path binary(basename + ".bin");
  bstfs::path logout(basename + "_out.log");
  bstfs::path logerr(basename + "_err.log");

  place_binary(tc->specimen(), binary);
  bstfs::permissions(binary, bstfs::owner_exe);

  const int   errcode = execute_binary(binary, logout, logerr, tc);

  // cleanup
  bstfs::remove(logerr);
  bstfs::remove(logout);
  bstfs::remove(binary);

  return ResultType(new LinuxExecutor::Result(errcode));
}

} // namespace
} // namespace
} // namespace
