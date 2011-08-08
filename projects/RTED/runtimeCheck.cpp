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

void
runtimeCheck(int argc, char** argv, const std::set<std::string>& rtedfiles, bool withupc)
{
   // PARSE AND TRANSFORM - 1rst round--------------------------------
   // Init Transformation object
   RtedTransformation rted(withupc, rtedfiles);
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
  bool                  withupc;

  FileRegistrar()
  : filenames(), withupc(false)
  {}

  void operator()(const char* arg)
  {
    filenames.insert( boostfs::system_complete( arg ).file_string() );
    withupc = withupc || boost::ends_with( std::string(arg), std::string(".upc") );
  }
};

int main(int argc, char** argv)
{
   // INIT -----------------------------------------------------
   // call RTED like this:
   const std::string invocation = "runtimeCheck FILES ROSEARGS\n"
                                  "  FILES    ... list of project files\n"
                                  "  ROSEARGS ... compiler/rose related arguments\n";

   if (argc < 2) { //7
      std::cerr << invocation << std::endl;
      exit(0);
   }

   char**                firstfile = argv+1;
   char**                lastfile = std::find_if(firstfile, argv+argc, notAFile);
   FileRegistrar         registrar = std::for_each(firstfile, lastfile, FileRegistrar() );

   // files are pushed on through bash script
   if (RTEDDEBUG)
      std::cerr << " >>>>>>>>>>>>>>>>>>>> NR OF FILES :: " << registrar.filenames.size() << std::endl;

   for (int i=1; i< argc; ++i) {
      std::cout << i << " : " << argv[i] << std::endl;
   }

   runtimeCheck(argc, argv, registrar.filenames, registrar.withupc);
   return 0;
}
#endif
