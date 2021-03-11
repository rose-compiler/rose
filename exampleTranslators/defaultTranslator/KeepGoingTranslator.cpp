/*
There are several levels of keep going support

1. GNU make's -k option

make will try to compile every file that can be tried and show as many compilation errors as possible

2. ROSE's buildin -rose:keep_going support

Similar to GNU Make's --keep-going option.

If ROSE encounters an error while processing your
input code, ROSE will simply run your backend compiler on
your original source code file, as is, without modification.

This is useful for compiler tests. For example,
when compiling a 100K LOC application, you can
try to compile as much as possible, ignoring failures,
in order to gauge the overall status of your translator,
with respect to that application.

3. src/frontend/SageIII/sage_support/keep_going.h /.cpp

 Rose::KeepGoing namespace provides support for creating a translator,
 which internally uses -rose:keep_going and keeps track of successfully processed or failed files, and save such information into log files. 

Original Author: Justin Too

Refactored and documented: Liao, 4/13/2017

*/
#include "rose.h"
#include "keep_going.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int
main(int argc, char * argv[])
{
    ROSE_INITIALIZE;

  // Using home may be a better choice, no scattered log files in every subdirectories.  
  //  report_filename__fail(boost::filesystem::path(getenv("HOME")).native()+"/rose-failed_files.txt");
  //  report_filename__pass(boost::filesystem::path(getenv("HOME")).native()+"/rose-passed_files.txt");
  Rose::KeepGoing::report_filename__fail = "rose-failed_files.txt";
  Rose::KeepGoing::report_filename__pass="rose-passed_files.txt";
//  Rose::KeepGoing::expectations_filename__fail="";
//  Rose::KeepGoing::expectations_filename__pass="";
//  Rose::KeepGoing::path_prefix="";


  //-----------command line processing -----------------
  /*
     "  --list-filenames                Prints the filename(s), computed by the ROSE commandline handling,\n"
     "                                  onto standard out. Each filename is separated by a new line.\n"
     "\n"
     "  --report-pass=<filename>        File to write report of passes\n"
     "  --report-fail=<filename>        File to write report of failures\n"
     "  --expected-failures=<filename>  File containing filenames that are expected to fail\n"
     "  --expected-passes=<filename>    File containing filenames that are expected to pass\n"
     "  --strip-path-prefix=<filename>  Normalize filenames by stripping this path prefix from them\n"
     "\n"
     "  --enable-ast-tests              Enables the internal ROSE AST consistency tests\n"
     "\n"
     "  -h,--help                       Show this help message\n"
     "  --verbose                       Enables debugging output, e.g. outputs successful files\n"
     "  --silent                        Disables debugging output\n"
     */
  // strip off some options used to guide keep-going features
  std::vector<std::string> stripped_cmdline;
  Rose::KeepGoing::commandLineProcessing (argc, argv, stripped_cmdline); 

  // Build the AST used by ROSE
  SgProject* project = frontend(stripped_cmdline);

  // this part is used to catch midend errors.
  if (KEEP_GOING_CAUGHT_MIDEND_SIGNAL)
  {
    std::cout
      << "[WARN] "
      << "Configured to keep going after catching a signal in the Midend"
      << std::endl;
    project->set_midendErrorCode(100);
  }
  else // put your midend code here
  {
    if (Rose::KeepGoing::enable_ast_tests)
    {
      // Run internal consistency tests on AST
      AstTests::runAllTests(project);
    }
    else
    {
      if (Rose::KeepGoing::verbose)
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
  std::vector<std::string> orig_rose_cmdline(argv, argv+argc);
  Rose::KeepGoing::generate_reports (project, orig_rose_cmdline);

  // IMPORTANT: Note that this is split into two steps, with generate_reports() in between;
  // Otherwise the backend errors will not be reported in the file report. 
  // return backend(project); 
  return backend_status;
}

