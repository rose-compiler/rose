#include <assert.h>
#include <setjmp.h>
#include <signal.h>

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

#include "rose.h"

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
CreateExpectedFailuresMap(const std::string& filename);

sigjmp_buf rose__midend_mark;
static void HandleMidendSignal(int sig);

int
main(int argc, char * argv[])
{
  std::string report_filename("rose-file_errors.txt");
  std::string expectations_filename;
  std::string path_prefix;
  bool        verbose;

  std::string program_name(argv[0]);

  std::vector<std::string> rose_cmdline;
  rose_cmdline.push_back(program_name);
  rose_cmdline.push_back("-rose:keep_going");

  {// CLI
      std::string cli_report = "--report=";
      std::string cli_expectations = "--expectations=";
      std::string cli_strip_path_prefix = "--strip-path-prefix=";
      std::string cli_verbose = "--verbose";

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
                  report_filename = arg;
              }
          }
          // --expectations
          else if (arg.find(cli_expectations) == 0)
          {
              arg.replace(0, cli_expectations.length(), "");
              if (arg.empty())
              {
                  std::cerr
                      << "[ERROR] "
                      << "[" << program_name << "] "
                      << "--expectations requires an argument, see --help"
                      << std::endl;
                  return 1;
              }
              else
              {
                  expectations_filename = arg;
                  if (!boost::filesystem::exists(expectations_filename))
                  {
                      std::cerr
                          << "[FATAL] "
                          << "Expectations file does not exist: "
                          << expectations_filename
                          << std::endl;
                      abort();
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
          else
          {
              rose_cmdline.push_back(arg);
          }
      }

      if (verbose)
      {
          std::cout
              << "ROSE Commandline: "
              << boost::algorithm::join(rose_cmdline, " ")
              << std::endl;
      }
  }// CLI

  // Build the AST used by ROSE
  SgProject* project = frontend(rose_cmdline);

  struct sigaction act;
  act.sa_handler = HandleMidendSignal;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGSEGV, &act, 0);
  sigaction(SIGABRT, &act, 0);

  if (sigsetjmp(rose__midend_mark, 0) == -1)
  {
      std::cout
          << "[WARN] Ignoring midend failure "
          << " as directed by -rose:keep_going"
          << std::endl;
      project->set_midendErrorCode(-1);
  }
  else
  {
      // Run internal consistency tests on AST
      AstTests::runAllTests(project);
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
              << "'" << filename << "'"
              << std::endl;
      }

      std::stringstream ss;
      ss
          //<< "[ERROR] "
          //<< "ROSE encountered an error while processing this file: "
          //<< "'" << filename << "'";
          << filename;

      AppendToFile(report_filename, ss.str());
  }

  if (verbose)
  {
      // Report successes
      SgFilePtrList files_without_errors = project->get_files_without_errors();
      std::cout
          << "[INFO] "
          << "ROSE successfully compiled "
          << "'" << files_without_errors.size() << "' "
          << "files"
          << std::endl;
      BOOST_FOREACH(SgFile* file, files_without_errors)
      {
          std::string filename = file->getFileName();
          filename = StripPrefix(path_prefix, filename);

          std::cout
              << "[INFO] "
              << "ROSE successfully compiled this file: "
              << "'" << filename << "'"
              << std::endl;
      }
  }

  if (!expectations_filename.empty())
  {
      std::map<std::string, std::string> expected_failures =
          CreateExpectedFailuresMap(expectations_filename);
      assert(expected_failures.empty() == false);

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
                  << "Unexpected failure for file: "
                  << "'" << filename << "'"
                  << std::endl;
              abort();
          }
          else
          {
              if (verbose)
              {
                  std::cout
                      << "[INFO] "
                      << "Expected failure for file: "
                      << "'" << filename << "'"
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
    << "  --report=<filename>             File to write error report\n"
    << "  --expectations=<filename>       File containing filenames that are expected to fail\n"
    << "  --strip-path-prefix=<filename>  Normalize filenames by stripping this path prefix from them\n"
    << "\n"
    << "  -h,--help                       Show this help message\n"
    << "  --verbose                       Enables debugging output, e.g. outputs successful files\n"
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


void
AppendToFile(const std::string& filename, const std::string& msg)
{
  std::ofstream fout(filename.c_str(), std::ios::app);
  if(!fout.is_open())
  {
      std::cerr
          << "[FATAL] "
          << "Couldn't open "
          << "'" << filename << "'"
          << std::endl;
      abort();
  }

  fout
      //<< "[" << GetTimestamp() << "] "
      << msg
      << std::endl;

  fout.close();
}

std::map<std::string, std::string>
CreateExpectedFailuresMap(const std::string& filename)
{
  std::map<std::string, std::string> expected_failures;

  std::ifstream fin(filename.c_str());
  if(!fin.is_open())
  {
      std::cerr
          << "[FATAL] "
          << "Couldn't open "
          << "'" << filename << "'"
          << std::endl;
      abort();
  }
  else
  {
      std::string line;
      while (fin.good())
      {
          getline (fin, line);
          expected_failures[line] = line;
      }
  }

  fin.close();

  return expected_failures;
}

static void HandleMidendSignal(int sig)
{
  std::cout << "[WARN] Caught midend signal='" << sig << "'" << std::endl;
  siglongjmp(rose__midend_mark, -1);
}
