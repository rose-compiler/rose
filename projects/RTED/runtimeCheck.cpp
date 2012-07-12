/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * -----------------------------------------------------------*/
#include <rose.h>
// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <set>
#include <iostream>
/* driscoll6 (4/15/11) The default Boost::Filesystem version was
 * bumped from 2 to 3 in Boost 1.46. Use version 2 until we have time to
 * refactor the code for version 3. */
#include "boost/algorithm/string/predicate.hpp"

#define BOOST_FILESYSTEM_VERSION 2
#include "boost/filesystem/operations.hpp"

#include "DataStructures.h"
#include "RtedTransformation.h"

namespace boostfs = boost::filesystem;

template <class T, T init, char const* str, char const* help_text>
struct RtedBooleanOption
{
  T           val;
  char const* name;
  char const* help;

  RtedBooleanOption()
  : val(init), name(str), help(help_text)
  {}

  void set() { val = !init; }

  T& operator=(T newval) { val = newval; }
  operator T() const { return val; }
};

extern char const globalsInitOption[] = "globalsInitialized";
extern char const globalsInitHelp[]   = "sets the initializtion status of globals to true\n"
                                        "                    this is standard behavior in C/C99/UPC";


void
runtimeCheck(int argc, char** argv, const RtedFiles& rtedfiles, RtedOptions ropt)
{
   // PARSE AND TRANSFORM - 1rst round--------------------------------
   // Init Transformation object
   RtedTransformation rted(rtedfiles, ropt);
   // Start parsing the project and insert header files
   SgProject* project= NULL;

   if (RTEDDEBUG)
      std::cerr << "Parsing original files... " << std::endl;
   for (int i=0;i<argc;++i)
      std::cout << argv[i] << " " ;
   std::cout << std::endl;

   project = rted.parse(argc, argv);
   ROSE_ASSERT(project);

   // perform all necessary transformations (calls)
   if (RTEDDEBUG)
      std::cerr << "Conducting transformations... " << std::endl;
   rted.transform(project);

   // call backend and create a new rose_rose_filename.c source file
   if (RTEDDEBUG)
      std::cerr << "Calling ROSE backend... " << std::endl;
   backend(project);
}

/* -----------------------------------------------------------
 * Main Function for RTED
 * -----------------------------------------------------------*/

static inline
bool notAFile(const char* arg)
{
  return !boostfs::exists(arg);
}

struct FileRegistrar
{
  std::set<std::string> filenames;

  FileRegistrar()
  : filenames()
  {}

  void operator()(const char* arg)
  {
    filenames.insert( boostfs::system_complete( arg ).file_string() );
  }

  operator RtedFiles() { return filenames; }
};

struct OptionsRegistrar
{
  RtedBooleanOption<bool, false, globalsInitOption, globalsInitHelp> globalsInitialized;

  void set_option(const std::string& optname)
  {
    if (optname == globalsInitialized.name) globalsInitialized.set();
  }

  void operator()(const char* opt)
  {
    static const std::string opt_prefix("--RTED:");

    if (!boost::starts_with(opt, opt_prefix)) return;

    set_option(opt + opt_prefix.length());
  }

  operator RtedOptions() const
  {
    RtedOptions res;

    res.globalsInitialized = globalsInitialized;

    return res;
  }

  friend
  std::ostream& operator<<(std::ostream& os, const OptionsRegistrar&);
};

std::ostream& operator<<(std::ostream& os, const OptionsRegistrar& opt)
{
  os << opt.globalsInitialized.name << opt.globalsInitialized.help
     << std::endl;
}


int main(int argc, char** argv)
{
   // INIT -----------------------------------------------------
   // call RTED like this:
   const std::string invocation = "runtimeCheck FILES ROSEARGS\n"
                                  "  FILES            ... list of project files\n"
                                  "  [--RTED:options] ... list of RTED options\n"
                                  "  ROSEARGS         ... compiler/rose related arguments\n\n";

   if (argc < 2) { //7
      std::cerr << invocation
                << "  where RTED options is one of the following\n"
                << OptionsRegistrar() << std::endl;
      exit(0);
   }

   char** const          firstfile = argv+1;
   char** const          limitopt = argv+argc;
   char** const          firstopt = std::find_if( firstfile, limitopt, notAFile);
   RtedFiles             rtedFiles = std::for_each( firstfile, firstopt, FileRegistrar() );
   RtedOptions           rtedOptions = std::for_each( firstopt, limitopt, OptionsRegistrar() );

   // files are pushed on through bash script
   if (RTEDDEBUG)
      std::cerr << " >>>>>>>>>>>>>>>>>>>> NR OF FILES :: " << rtedFiles.size() << std::endl;

   for (int i=1; i< argc; ++i) {
      std::cout << i << " : " << argv[i] << std::endl;
   }

   runtimeCheck(argc, argv, rtedFiles, rtedOptions);
   return 0;
}
#endif
