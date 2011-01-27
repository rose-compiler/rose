/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * -----------------------------------------------------------*/
#include <rose.h>
// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <set>
#include <iostream>

#include "DataStructures.h"
#include "RtedTransformation.h"

#include "boost/filesystem/operations.hpp"

namespace boostfs = boost::filesystem;

void
runtimeCheck(int argc, char** argv, std::set<std::string>& rtedfiles) {
   // PARSE AND TRANSFORM - 1rst round--------------------------------
   // Init Transformation object
   RtedTransformation rted;
   // Start parsing the project and insert header files
   SgProject* project= NULL;

   if (RTEDDEBUG())
      std::cerr << "Parsing original files... " << std::endl;
   for (int i=0;i<argc;++i)
      std::cout << argv[i] << " " ;
   std::cout << std::endl;
   project = rted.parse(argc, argv);
   ROSE_ASSERT(project);

   // perform all necessary transformations (calls)
   if (RTEDDEBUG())
      std::cerr << "Conducting transformations... " << std::endl;
   rted.transform(project, rtedfiles);

   // call backend and create a new rose_rose_filename.c source file
   if (RTEDDEBUG())
      std::cerr << "Calling ROSE backend... " << std::endl;
   backend(project);
}

/* -----------------------------------------------------------
 * Main Function for RTED
 * -----------------------------------------------------------*/

int main(int argc, char** argv) {
  static const std::string invocation = "./runtimeCheck NRFILES FILES [-I...]";
   // INIT -----------------------------------------------------
   // call RTED like this:
   if (argc < 2) { //7
      std::cerr << invocation << std::endl;
      exit(0);
   }

   int nrfiles = (strtol(argv[1], NULL, 10));

   if (argc < nrfiles + 2) { //7
      std::cerr << "NRFILES < NO-OF-ARGUMENTS" << std::endl << std::endl;
      std::cerr << invocation << std::endl;
      exit(0);
   }

   std::set <std::string> rtedfiles;
   std::string            abs_path;

   for (int i=2; i< nrfiles + 2; ++i)
   {
     std::string filename = argv[i];
     const int   pos=filename.rfind("/");

     if (pos>0 && pos!=(int)std::string::npos) {
        abs_path = filename.substr(0,pos+1);
     }

     rtedfiles.insert( boostfs::system_complete( filename ).file_string() );
     if (RTEDDEBUG())
        std::cerr << i << ": >>>>> Found filename : " << filename << std::endl;
   }
   // files are pushed on through bash script
   //sort(rtedfiles.begin(), rtedfiles.end(),greater<std::string>());
   if (RTEDDEBUG())
      std::cerr << " >>>>>>>>>>>>>>>>>>>> NR OF FILES :: " << rtedfiles.size() << std::endl;

   for (int i=1; i< argc; ++i) {
      std::cout << i << " : " << argv[i] << std::endl;
   }

   if (RTEDDEBUG())
      std::cerr << "Running RTED in :" << abs_path << std::endl;

   runtimeCheck(argc-1, argv+1, rtedfiles);
   return 0;
}
#endif
