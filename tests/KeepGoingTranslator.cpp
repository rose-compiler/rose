#include "rose.h"
#include "keep_going.h"

#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h> //getpid()
#include <unistd.h>    //getpid()

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

static void
ShowUsage(std::string program_name);

/**
 * @returns The current time in the specified format
 */
std::string
GetTimestamp(const std::string& format = "%Y-%m-%d %H:%M:%S");

std::string
StripPrefix(const std::string& prefix, const std::string& str);

/**
 * @brief Append msg to filename
 */
void
AppendToFile(const std::string& filename, const std::string& msg);

/**
 * @returns A map of all filenames expected to fail.
 */
std::map<std::string, std::string>
CreateExpectationsMap(const std::string& filename);

/**
 * @returns A vector of all filenames from the commandline.
 */
std::vector<std::string>
GetSourceFilenamesFromCommandline(const std::vector<std::string>& argv);

int
main(int argc, char * argv[])
{
  bool verbose = false;
  bool enable_ast_tests = false;
  std::string report_filename__fail("rose-failed_files.txt");
  std::string report_filename__pass("rose-passed_files.txt");
  std::string expectations_filename__fail("");
  std::string expectations_filename__pass("");
  std::string path_prefix;

  std::string program_name(argv[0]);

  std::vector<std::string> rose_cmdline;
  rose_cmdline.push_back(program_name);
  rose_cmdline.push_back("-rose:keep_going");

  {// CLI
      std::string cli_list_filenames        = "--list-filenames"; // deprecated 2013-11-2
      std::string cli_report                = "--report="; // deprecated 2013-11-2
      std::string cli_report__fail          = "--report-fail=";
      std::string cli_report__pass          = "--report-pass=";
      std::string cli_expectations__fail    = "--expected-failures=";
      std::string cli_expectations__pass    = "--expected-passes=";
      std::string cli_strip_path_prefix     = "--strip-path-prefix=";
      std::string cli_enable_ast_tests      = "--enable-ast-tests";
      std::string cli_verbose               = "--verbose";
      std::string cli_silent                = "--silent";

      for (int ii = 1; ii < argc; ++ii)
      {
          std::string arg = argv[ii];

          // --help
          if ((arg == "-h") || (arg == "--help"))
          {
              ShowUsage(program_name);
              return 0;
          }
          // --verbose
          else if (arg.find(cli_verbose) == 0)
          {
              verbose = true;
          }
          // --silent
          else if (arg.find(cli_silent) == 0)
          {
              verbose = false;
          }
          // --list-filenames
          else if (arg.find(cli_list_filenames) == 0)
          {
              arg.replace(0, cli_list_filenames.length(), "");
              {
                  std::vector<std::string> filenames =
                      GetSourceFilenamesFromCommandline(
                          std::vector<std::string>(argv, argv + argc));
                  BOOST_FOREACH(std::string filename, filenames)
                  {
                      std::cout << filename << std::endl;
                  }
              }
              return 0;
          }
          // --report=<filename>
          else if (arg.find(cli_report) == 0)
          {
              arg.replace(0, cli_report.length(), "");
              if (arg.empty())
              {
                  std::cerr
                      << "[ERROR] "
                      << "[" << program_name << "] "
                      << "--report requires an argument, see --help"
                      << std::endl;
                  return 1;
              }
              else
              {
                  report_filename__fail = arg + "-fail";
                  report_filename__pass = arg + "-pass";
              }
          }
          // --report-fail=<filename>
          else if (arg.find(cli_report__fail) == 0)
          {
              arg.replace(0, cli_report__fail.length(), "");
              if (arg.empty())
              {
                  std::cerr
                      << "[ERROR] "
                      << "[" << program_name << "] "
                      << "--report-fail requires an argument, see --help"
                      << std::endl;
                  return 1;
              }
              else
              {
                  report_filename__fail = arg;
              }
          }
          // --report-pass=<filename>
          else if (arg.find(cli_report__pass) == 0)
          {
              arg.replace(0, cli_report__pass.length(), "");
              if (arg.empty())
              {
                  std::cerr
                      << "[ERROR] "
                      << "[" << program_name << "] "
                      << "--report-pass requires an argument, see --help"
                      << std::endl;
                  return 1;
              }
              else
              {
                  report_filename__pass = arg;
              }
          }
          // --expected-failures
          else if (arg.find(cli_expectations__fail) == 0)
          {
              arg.replace(0, cli_expectations__fail.length(), "");
              if (arg.empty())
              {
                  std::cerr
                      << "[ERROR] "
                      << "[" << program_name << "] "
                      << "--expected-failures requires an argument, see --help"
                      << std::endl;
                  return 1;
              }
              else
              {
                  expectations_filename__fail = arg;
                  if (!boost::filesystem::exists(expectations_filename__fail))
                  {
                      std::cerr
                          << "[FATAL] "
                          << "Expected failures file does not exist: "
                          << expectations_filename__fail
                          << std::endl;
                      exit(1);
                  }
              }
          }
          // --expected-passes
          else if (arg.find(cli_expectations__pass) == 0)
          {
              arg.replace(0, cli_expectations__pass.length(), "");
              if (arg.empty())
              {
                  std::cerr
                      << "[ERROR] "
                      << "[" << program_name << "] "
                      << "--expected-passes requires an argument, see --help"
                      << std::endl;
                  return 1;
              }
              else
              {
                  expectations_filename__pass = arg;
                  if (!boost::filesystem::exists(expectations_filename__pass))
                  {
                      std::cerr
                          << "[FATAL] "
                          << "expected passes file does not exist: "
                          << expectations_filename__pass
                          << std::endl;
                      exit(1);
                  }
              }
          }
          // --strip-path-prefix
          else if (arg.find(cli_strip_path_prefix) == 0)
          {
              arg.replace(0, cli_strip_path_prefix.length(), "");
              if (arg.empty())
              {
                  std::cerr
                      << "[ERROR] "
                      << "[" << program_name << "] "
                      << "--strip-path-prefix requires an argument, see --help"
                      << std::endl;
                  return 1;
              }
              else
              {
                  path_prefix = arg;
              }
          }
          // --enable-ast-tests
          else if (arg.find(cli_enable_ast_tests) == 0)
          {
              enable_ast_tests = true;
          }
          else
          {
              rose_cmdline.push_back(arg);
          }
      }

      if (verbose)
      {
          std::cout
              << "[INFO] ROSE Commandline: "
              << boost::algorithm::join(rose_cmdline, " ")
              << std::endl;
      }
  }// CLI

  // Build the AST used by ROSE
  SgProject* project = frontend(rose_cmdline);

  if (KEEP_GOING_CAUGHT_MIDEND_SIGNAL)
  {
      std::cout
          << "[WARN] "
          << "Configured to keep going after catching a signal in the Midend"
          << std::endl;
      project->set_midendErrorCode(100);
  }
  else
  {
      if (enable_ast_tests)
      {
          // Run internal consistency tests on AST
          AstTests::runAllTests(project);
      }
      else
      {
          if (verbose)
          {
              std::cerr
                  << "[INFO] "
                  << "Skipping AST consistency tests; turn them on with '--enable-ast-tests'"
                  << std::endl;
          }
      }
  }

  // Insert your own manipulation of the AST here...

  // Generate source code from the AST and call the vendor's compiler
  int backend_status = backend(project);

  // Report errors
  SgFilePtrList files_with_errors = project->get_files_with_errors();
  BOOST_FOREACH(SgFile* file, files_with_errors)
  {
      std::string filename = file->getFileName();
                  filename = StripPrefix(path_prefix, filename);

      if (verbose)
      {
          std::cout
              << "[ERROR] "
              << "ROSE encountered an error while processing this file: "
              << "'" << path_prefix << filename << "'"
              << std::endl;
      }

      // <file> <frontend> <unparser> <backend>
      std::stringstream ss;
      ss << filename << " "
         << file->get_frontendErrorCode() << " "
         << file->get_javacErrorCode() << " "
         << file->get_unparserErrorCode() << " "
         << file->get_backendCompilerErrorCode() << " "
         << file->get_unparsedFileFailedCompilation();

      AppendToFile(report_filename__fail, ss.str());
  }

  // Report successes
  SgFilePtrList files_without_errors = project->get_files_without_errors();
  BOOST_FOREACH(SgFile* file, files_without_errors)
  {
      std::string filename = file->getFileName();
                  filename = StripPrefix(path_prefix, filename);

      if (verbose)
      {
          std::cout
              << "[INFO] "
              << "ROSE successfully compiled this file: "
              << "'" << path_prefix << filename << "'"
              << std::endl;
      }

      std::stringstream ss;
      ss << filename;

      AppendToFile(report_filename__pass, ss.str());
  }

  if (!expectations_filename__fail.empty())
  {
      std::map<std::string, std::string> expected_failures =
          CreateExpectationsMap(expectations_filename__fail);

      // TOO1 (3/5/2014): Only works if all files are processed by this single commandline.
      //if(files_with_errors.size() != expected_failures.size())
      //{
      //    std::cerr
      //        << "[FATAL] "
      //        << "Expected '" << expected_failures.size() << "' failures, but "
      //        << "encountered '" << files_with_errors.size() << "' failures"
      //        << std::endl;
      //    exit(1);
      //}

      BOOST_FOREACH(SgFile* file, files_with_errors)
      {
          std::string filename = file->getFileName();
          filename = StripPrefix(path_prefix, filename);

          std::map<std::string, std::string>::iterator it =
              expected_failures.find(filename);
          if (it == expected_failures.end())
          {
              std::cerr
                  << "[FATAL] "
                  << "File failed unexpectedly: "
                  << "'" << path_prefix << filename << "'"
                  << std::endl;
              exit(1);
          }
          else
          {
              if (verbose)
              {
                  std::cout
                      << "[INFO] "
                      << "File failed as expected: "
                      << "'" << path_prefix << filename << "'"
                      << std::endl;
              }
          }
      }
  }

  if (!expectations_filename__pass.empty())
  {
      std::map<std::string, std::string> expected_passes =
          CreateExpectationsMap(expectations_filename__pass);

      // TOO1 (3/5/2014): Only works if all files are processed by this single commandline.
      //
      //if(files_without_errors.size() != expected_passes.size())
      //{
      //    std::cerr
      //        << "[FATAL] "
      //        << "Expected '" << expected_passes.size() << "' passes, but "
      //        << "encountered '" << files_without_errors.size() << "' passes"
      //        << std::endl;
      //    exit(1);
      //}

      BOOST_FOREACH(SgFile* file, files_without_errors)
      {
          std::string filename = file->getFileName();
          filename = StripPrefix(path_prefix, filename);

          std::map<std::string, std::string>::iterator it =
              expected_passes.find(filename);
          if (it == expected_passes.end())
          {
              std::cerr
                  << "[WARN] "
                  << "File passed unexpectedly: "
                  << "'" << path_prefix << filename << "'"
                  << std::endl;
          }
          else
          {
              if (verbose)
              {
                  std::cout
                      << "[INFO] "
                      << "File passed as expected: "
                      << "'" << path_prefix << filename << "'"
                      << std::endl;
              }
          }
      }
  }

  return backend_status;
}

static void
ShowUsage(std::string program_name)
{
  std::cerr
    << "Usage: " << program_name << " [--help] [ROSE Commandline]\n"
    << "Options:\n"
    << "  --list-filenames                Prints the filename(s), computed by the ROSE commandline handling,\n"
    << "                                  onto standard out. Each filename is separated by a new line.\n"
    << "\n"
    << "  --report-pass=<filename>        File to write report of passes\n"
    << "  --report-fail=<filename>        File to write report of failurest\n"
    << "  --expected-failures=<filename>  File containing filenames that are expected to fail\n"
    << "  --expected-passes=<filename>    File containing filenames that are expected to pass\n"
    << "  --strip-path-prefix=<filename>  Normalize filenames by stripping this path prefix from them\n"
    << "\n"
    << "  --enable-ast-tests              Enables the internal ROSE AST consistency tests\n"
    << "\n"
    << "  -h,--help                       Show this help message\n"
    << "  --verbose                       Enables debugging output, e.g. outputs successful files\n"
    << "  --silent                        Disables debugging output\n"
    << std::endl;
}

std::string
StripPrefix(const std::string& prefix, const std::string& str)
{
  std::string stripped_str(str);

  int index = str.find(prefix, 0);
  if (index != std::string::npos)
  {
      stripped_str.replace(0, prefix.length(), "");
  }

  return stripped_str;
}

std::string
GetTimestamp(const std::string& format)
{
  using namespace boost::posix_time;

  ptime now = second_clock::universal_time();

  static std::locale loc(std::cout.getloc(),
                         new time_facet(format.c_str()));

  std::basic_stringstream<char> ss;
  ss.imbue(loc);
  ss << now;

  return ss.str();
}

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>

#include <iostream>
#include <string>

#include <cstdlib>

// http://chris-sharpe.blogspot.com/2013/05/better-than-systemtouch.html
void touch(const std::string& pathname)
{
    int fd = open(pathname.c_str(),
                  O_WRONLY|O_CREAT|O_NOCTTY|O_NONBLOCK,
                  0666);
    if (fd<0) // Couldn't open that path.
    {
        std::cerr
            << __PRETTY_FUNCTION__
            << ": Couldn't open() path \""
            << pathname
            << "\"\n";
        return;
    }

    int rc = utime(pathname.c_str(), 0);

    if (rc)
    {
        std::cerr
            << __PRETTY_FUNCTION__
            << ": Couldn't utime() path \""
            << pathname
            << "\"\n";
        return;
    }
    std::clog
        << __PRETTY_FUNCTION__
        << ": Completed touch() on path \""
        << pathname
        << "\"\n";
}


void
AppendToFile(const std::string& filename, const std::string& msg)
{
  touch(filename);

  boost::interprocess::file_lock flock;
  try
  {
      boost::interprocess::file_lock* flock_tmp =
          new boost::interprocess::file_lock(filename.c_str());
      flock.swap(*flock_tmp);
      delete flock_tmp;
      flock.lock();
  }
  catch (boost::interprocess::interprocess_exception &ex)
  {
      std::cout << ex.what() << std::endl;

      std::cerr
          << "[FATAL] "
          << "Couldn't lock "
          << "'" << filename << "'"
          << std::endl;

      exit(1);
  }

  std::ofstream fout(filename.c_str(), std::ios::app);

  if(!fout.is_open())
  {
      std::cerr
          << "[FATAL] "
          << "Couldn't open "
          << "'" << filename << "'"
          << std::endl;
      flock.unlock();
      exit(1);
  }

  fout
      << GetTimestamp()  << " "
      << getpid() << " "
      << msg
      << std::endl;

  fout.close();
  flock.unlock();
}

std::map<std::string, std::string>
CreateExpectationsMap(const std::string& filename)
{
  std::map<std::string, std::string> expectations;

  std::ifstream fin(filename.c_str());
  if(!fin.is_open())
  {
      std::cerr
          << "[FATAL] "
          << "Couldn't open "
          << "'" << filename << "'"
          << std::endl;
      exit(1);
  }
  else
  {
      std::string line;
      while (fin.good())
      {
          getline (fin, line);
          if (line.size() > 1)
          {
              expectations[line] = line;
          }
      }
  }

  fin.close();

  return expectations;
}

std::vector<std::string>
GetSourceFilenamesFromCommandline(const std::vector<std::string>& argv)
{
  std::vector<std::string> filenames =
      CommandlineProcessing::generateSourceFilenames(argv, false);
  return filenames;
}

