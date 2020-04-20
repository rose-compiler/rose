/**
 * \file    signal_handling.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    November 16, 2013
 */
#include "sage3basic.h"

#include <iostream>

#include "keep_going.h"
#include "processSupport.h" // ROSE_ASSERT in ROSE/src/util

#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#ifdef __linux__
#include <utime.h>
#endif
#include <boost/algorithm/string/join.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

using namespace std; 
using namespace boost::interprocess; 

namespace Rose {
namespace KeepGoing {
  bool g_keep_going = false;

 bool verbose = false;
 bool enable_ast_tests= false;

 std::string report_filename__fail;
 std::string report_filename__pass;
 std::string expectations_filename__fail;
 std::string expectations_filename__pass;
 std::string path_prefix;

 std::map <SgFile* , std::string> File2StringMap; 

#ifndef _MSC_VER
struct sigaction SignalAction;
#endif //_MSC_VER

bool set_signal_handler(SignalHandlerFunction handler)
{
#ifndef _MSC_VER
    SignalAction.sa_flags   = 0;
    SignalAction.sa_handler = handler;

    sigemptyset(&SignalAction.sa_mask);

    sigaction(SIGSEGV, &SignalAction, 0);
    sigaction(SIGABRT, &SignalAction, 0);
#else
    ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
#endif //_MSC_VER

    return true;
}

namespace Frontend {
  #ifndef _MSC_VER
    sigjmp_buf jmp_target;
  #endif //_MSC_VER
  void SignalHandler(int sig)
  {
      std::cout
          << "[WARN] "
          << "Caught signal="
#ifndef _MSC_VER
          << "'" << strsignal(sig) << "' "
#else
          << "'" << sig << "' "
#endif
          << "during frontend processing"
          << std::endl;

      #ifndef _MSC_VER
          siglongjmp(Frontend::jmp_target, -1);
      #else
          ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
      #endif //_MSC_VER
  }

  namespace Commandline {
    #ifndef _MSC_VER
      sigjmp_buf jmp_target;
    #endif //_MSC_VER
    void SignalHandler(int sig)
    {
        std::cout
            << "[WARN] "
            << "Caught signal="
#ifndef _MSC_VER
          << "'" << strsignal(sig) << "' "
#else
          << "'" << sig << "' "
#endif
            << "during commandline processing in frontend processing"
            << std::endl;

        #ifndef _MSC_VER
            siglongjmp(Frontend::Commandline::jmp_target, -1);
        #else
            ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
        #endif //_MSC_VER
    }
  }// ::Rose::KeepGoing::Frontend::Commandline

  namespace SecondaryPass {
    #ifndef _MSC_VER
      sigjmp_buf jmp_target;
    #endif //_MSC_VER
    void SignalHandler(int sig)
    {
        std::cout
            << "[WARN] "
            << "Caught signal="
#ifndef _MSC_VER
          << "'" << strsignal(sig) << "' "
#else
          << "'" << sig << "' "
#endif
            << "during secondary pass in frontend processing"
            << std::endl;

        #ifndef _MSC_VER
            siglongjmp(Frontend::SecondaryPass::jmp_target, -1);
        #else
            ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
        #endif //_MSC_VER
    }
  }// ::Rose::KeepGoing::Frontend::SecondaryPass
}// ::Rose::KeepGoing::Frontend

namespace Midend {
  #ifndef _MSC_VER
    sigjmp_buf jmp_target;
  #endif //_MSC_VER
  void SignalHandler(int sig)
  {
      std::cout
          << "[WARN] "
          << "Caught signal="
#ifndef _MSC_VER
          << "'" << strsignal(sig) << "' "
#else
          << "'" << sig << "' "
#endif
          << "during midend processing"
          << std::endl;

      #ifndef _MSC_VER
          siglongjmp(Midend::jmp_target, -1);
      #else
          ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
      #endif //_MSC_VER
  }
}// ::Rose::KeepGoing::Midend

namespace Backend 
{
  namespace Unparser {
    #ifndef _MSC_VER
      sigjmp_buf jmp_target;
    #endif //_MSC_VER
    void SignalHandler(int sig)
    {
        std::cout
            << "[WARN] "
            << "Caught signal="
#ifndef _MSC_VER
          << "'" << strsignal(sig) << "' "
#else
          << "'" << sig << "' "
#endif
            << "during backend unparser processing"
            << std::endl;
  
        #ifndef _MSC_VER
            siglongjmp(Backend::Unparser::jmp_target, -1);
        #else
            ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
        #endif //_MSC_VER
    }
  }// ::Rose::KeepGoing::Backend::Unparser
  
  namespace Compiler {
    #ifndef _MSC_VER
      sigjmp_buf jmp_target;
    #endif //_MSC_VER
    void SignalHandler(int sig)
    {
        std::cout
            << "[WARN] "
            << "Caught signal="
#ifndef _MSC_VER
          << "'" << strsignal(sig) << "' "
#else
          << "'" << sig << "' "
#endif
            << "during backend compiler processing"
            << std::endl;
  
        #ifndef _MSC_VER
            siglongjmp(Backend::Compiler::jmp_target, -1);
        #else
            ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
        #endif //_MSC_VER
    }
  }// ::Rose::KeepGoing::Backend::Compiler
}// ::Rose::KeepGoing::Backend
}// ::Rose::KeepGoing
}// Rose

// TOO1 (05/14/2013): Handling for -rose:keep_going
//
// Compile the original source code file if:
//
// 1. Unparsing was skipped
// 2. The frontend encountered any errors, and the user specified to
//    "keep going" with -rose:keep_going.
//
//    Warning: Apparently, a frontend error code <= 3 indicates an EDG
//    frontend warning; however, existing logic says nothing about the
//    other language frontends' exit statuses.
bool
Rose::KeepGoing::Backend::UseOriginalInputFile(SgFile* file)
{
  ROSE_ASSERT(file != NULL);

#if 0
  // DQ (11/10/2016): Debugging information (detected that p_keep_going data member was uninitialized).
     printf ("file->get_unparse_output_filename().empty() = %s \n",file->get_unparse_output_filename().empty() ? "true" : "false");
     printf ("file->get_frontendErrorCode() != 0          = %s \n",(file->get_frontendErrorCode() != 0) ? "true" : "false");
     printf ("file->get_project()->get_midendErrorCode()  = %s \n",(file->get_project()->get_midendErrorCode()) ? "true" : "false");
     printf ("file->get_unparserErrorCode()               = %s \n",(file->get_unparserErrorCode()) ? "true" : "false");
     printf ("file->get_backendCompilerErrorCode() != 0   = %s \n",(file->get_backendCompilerErrorCode() != 0) ? "true" : "false");
     printf ("file->get_project()->get_keep_going()       = %s \n",file->get_project()->get_keep_going() ? "true" : "false");
#endif

  return
    // (1) An ROSE unparsed file was not generated
    file->get_unparse_output_filename().empty() == true ||
    // (2) File has an error and user specified to "keep going"
    (
        (
            file->get_frontendErrorCode() != 0 ||
            file->get_project()->get_midendErrorCode() != 0 ||
            file->get_unparserErrorCode() != 0 ||
            file->get_backendCompilerErrorCode() != 0) &&
        (
            file->get_project()->get_keep_going()
        )
    );
}// ::Rose::KeepGoing::Backend::UseOriginalInputFile


// Move many helper functions here to simplify writing a translator with keep-going logging support, Liao 4/12/2017

//! Processing command line options
void Rose::KeepGoing::commandLineProcessing 
                          (int argc, char* argv[],
                           std::vector<std::string> & rose_cmdline
                           ) 
{
  std::string program_name(argv[0]);
  rose_cmdline.push_back(program_name);
  // wrap the -rose:keep_going option inside of this translator
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
        exit(0);
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
        exit(0);
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
          exit(1);
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
          exit(1);
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
          exit(1);
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
          exit(1);
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
          exit(1);
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
          exit(1);
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

}

void Rose::KeepGoing::setMidendErrorCode (SgProject* project, int errorCode)
{
  project->set_midendErrorCode(errorCode);

  SgFilePtrList files = project->get_files();
  BOOST_FOREACH(SgFile* file, files)
  {
    file->set_midendErrorCode(errorCode);
  }
}

void Rose::KeepGoing::generate_reports(SgProject* project, 
                     std::vector< std::string> orig_rose_cmdline
                       )
{
  // --------------------------------------------------------------
  SgFilePtrList files_with_errors = project->get_files_with_errors();
  
  // Convert command line options into a single line string
  std::string orig_command_str; 
  for (size_t len= 0; len<orig_rose_cmdline.size(); len++)
  {
    if (len!=0)
      orig_command_str +=" ";
    orig_command_str += orig_rose_cmdline[len];
  }
  orig_command_str +="\n";

  // add original command line into the log file so users can easily reproduce the errors. 
  if (files_with_errors.size()>0 && report_filename__fail.size()>0)
  {
    AppendToFile (report_filename__fail, "------------------------\n");
    // add time stamp in the beginning
    std::ostringstream ostamp;
    ostamp << GetTimestamp()  << " " << getpid() << std::endl;
    AppendToFile (report_filename__fail, ostamp.str());

    AppendToFile (report_filename__fail, orig_command_str);

    BOOST_FOREACH(SgFile* file, files_with_errors)
    {
      std::string filename = file->getFileName();
      filename = StripPrefix(path_prefix, filename);

      std::stringstream ss;
      ss <<"Processed File: With Errors " <<  filename << endl; // Help diagnosis , output full file name, even command lines may have partial file paths
      if (verbose)
      {
        std::cout
          << "[ERROR] "
          << "ROSE encountered an error while processing this file: "
          << "'" << path_prefix << filename << "'"
          << std::endl;
      }

      // <file> <frontend> <unparser> <backend>
      // Keep all info. of one file into one line. Users can easily count the total failures. 
      if (file->get_frontendErrorCode())
        ss << "\t Frontend Error Code:" << file->get_frontendErrorCode() ;
      if (file->get_midendErrorCode())
        ss << "\t Midend Error Code:" << file->get_midendErrorCode() ;
      if (file->get_javacErrorCode())    
        ss << "\t JAVA Error Code:"<< file->get_javacErrorCode(); 
      if (file->get_unparserErrorCode())   
        ss << "\t Unparser Error Code:"<< file->get_unparserErrorCode();
      if (file->get_backendCompilerErrorCode())   
        ss << "\t Backend Compiler Error Code: " << file->get_backendCompilerErrorCode();
      if (file->get_unparsedFileFailedCompilation())   
        ss << "\t Unparsed File Failed Compilation Code: "<< file->get_unparsedFileFailedCompilation();
      ss<<"\n";  
      AppendToFile(report_filename__fail, ss.str());

      //Sometimes even for files failed on backend stage, some analysis results are generated. 
      //we still want to output such results.
      // If exists, output the analysis results associated with each file
      std::ostringstream oss;
      oss <<  File2StringMap[file]; // not copyable, not assignable
      if (oss.str().size()>0)
      {
        AppendToFile(report_filename__fail, oss.str());
      }
    }
  }
  // Report successes
  SgFilePtrList files_without_errors = project->get_files_without_errors();
  if (files_without_errors.size()>0 && report_filename__pass.size()>0)
  {
   // full command line , it may has multiple files 
   
    AppendToFile (report_filename__pass, "------------------------\n");
    //AppendToFile (report_filename__pass, orig_command_str);

    // add time stamp in the beginning
    std::ostringstream ostamp ;
    ostamp << GetTimestamp()  << " " << getpid() << std::endl;
    AppendToFile (report_filename__pass, ostamp.str());

    BOOST_FOREACH(SgFile* file, files_without_errors)
    {
      std::string full_filename = file->getFileName();
      std::string filename = StripPrefix(path_prefix, full_filename);

      // output file full path first
      AppendToFile(report_filename__pass, "Processed File: Without Errors:"+full_filename+"\n");
      if (verbose)
      {
        std::cout
          << "[INFO] "
          << "ROSE successfully compiled this file: "
          << "'" << path_prefix << filename << "'"
          << std::endl;
      }

      // If exists, output the analysis results associated with each file
      std::ostringstream oss;
      oss <<  File2StringMap[file]; // not copyable, not assignable
      if (oss.str().size()>0)
      {

        AppendToFile(report_filename__pass, oss.str());
      }
    }
  }

  //Sometimes even for files failed on backend stage, some analysis results are generated. 
  //we still want to output such results.
  // If exists, output the analysis results associated with each file
  // Only merge the results into the pass log file if the fail log file is not specified!!
  if (files_with_errors.size()>0 && report_filename__pass.size()>0 && report_filename__fail.empty())
  {
    bool runonce = false; 
    // add time stamp in the beginning
    std::ostringstream ostamp;
    ostamp << GetTimestamp()  << " " << getpid() << std::endl;

    BOOST_FOREACH(SgFile* file, files_with_errors)
    {
      std::string filename = file->getFileName();

      //Sometimes even for files failed on backend stage, some analysis results are generated. 
      //we still want to output such results.
      // If exists, output the analysis results associated with each file
      std::ostringstream oss;
      oss <<  File2StringMap[file]; // not copyable, not assignable
      if (oss.str().size()>0)
      {
        if (!runonce)
        {
          AppendToFile (report_filename__pass, "------Analysis results for files with backend errors----------------------\n");
          AppendToFile (report_filename__pass, ostamp.str());
          runonce = true; 
        }

        AppendToFile(report_filename__pass, "Processed File: With Backend Errors:"+filename+"\n");
        AppendToFile(report_filename__pass, oss.str());
      }
    }
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
}

void
Rose::KeepGoing::ShowUsage(std::string program_name)
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
Rose::KeepGoing::StripPrefix(const std::string& prefix, const std::string& str)
{
  std::string stripped_str(str);

  size_t index = str.find(prefix, 0);
  if (index != std::string::npos)
  {
      stripped_str.replace(0, prefix.length(), "");
  }

  return stripped_str;
}

std::string
Rose::KeepGoing::GetTimestamp(const std::string& format)
{
  using namespace boost::posix_time;

  //ptime now = second_clock::universal_time();
  // Using local time instead to be user-friendly.
  ptime now = second_clock::local_time();

  static std::locale loc(std::cout.getloc(),
                         new time_facet(format.c_str()));

  std::basic_stringstream<char> ss;
  ss.imbue(loc);
  ss << now;

  return ss.str();
}

void Rose::KeepGoing::touch(const std::string& pathname)
{
  
#ifdef __linux__  // commented out to pass windows test for now
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
   if (verbose)
   {
    std::clog
        << __PRETTY_FUNCTION__
        << ": Completed touch() on path \""
        << pathname
        << "\"\n";
   }
#endif
}

//! AppendToFile a message
void
Rose::KeepGoing::AppendToFile(const std::string& filename, const std::string& msg)
{
  // if filename is NULL, no action is needed. 
  if (filename.size()==0)
    return; 
 // use a separated file for the file lock
  string lock_file_name = filename+".lock"; 
  touch(filename);
  touch(lock_file_name); //this lock file must exist. Or later flock() will fail.

  boost::interprocess::file_lock flock (lock_file_name.c_str());
  // introduce a scope to use the scoped lock, which automatically unlock when existing the scope
  {
    boost::interprocess::scoped_lock<file_lock> e_lock(flock);
    std::ofstream fout(filename.c_str(), std::ios::app);
    if(!fout.is_open())
    {
      std::cerr
        << "[FATAL] "
        << "Couldn't open "
        << "'" << filename << "'"
        << std::endl;
    }
    else
    {  
#if 0
      fout
        << GetTimestamp()  << " "
        << getpid() << " "
        << msg
        << std::endl;
#endif
      fout<<msg; 
      fout.flush();
      fout.close();
    }
  } // end scope for the scoped lock
  boost::filesystem::remove (lock_file_name);
}

std::map<std::string, std::string>
Rose::KeepGoing::CreateExpectationsMap(const std::string& filename)
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

/**
 * @returns A vector of all filenames from the commandline.
 */
std::vector<std::string>
Rose::KeepGoing::GetSourceFilenamesFromCommandline(const std::vector<std::string>& argv)
{
  std::vector<std::string> filenames =
      CommandlineProcessing::generateSourceFilenames(argv, false);
  return filenames;
}
