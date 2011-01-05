/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * -----------------------------------------------------------*/
#include <rose.h>
// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE
#include <string>
#include "DataStructures.h"
#include "RtedTransformation.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include <vector>
#include <set>
#include <iostream>
#include <sys/stat.h>
#include "boost/filesystem/operations.hpp"
#include <unistd.h>

using namespace boost;
using namespace std;
using namespace boost::filesystem;

void
runtimeCheck(int argc, char** argv, set<string>& rtedfiles) {
   // PARSE AND TRANSFORM - 1rst round--------------------------------
   // Init Transformation object
   RtedTransformation rted;
   // Start parsing the project and insert header files
   SgProject* project= NULL;

   if (RTEDDEBUG())
      cerr << "Parsing original files... " << endl;
   for (int i=0;i<argc;++i)
      cout << argv[i] << " " ;
   cout << endl;
   project = rted.parse(argc, argv);
   ROSE_ASSERT(project);

   // perform all necessary transformations (calls)
   if (RTEDDEBUG())
      cerr << "Conducting transformations... " << endl;
   rted.transform(project, rtedfiles);

   // call backend and create a new rose_rose_filename.c source file
   if (RTEDDEBUG())
      cerr << "Calling ROSE backend... " << endl;
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

   int nrfiles = static_cast<int>(strtol(argv[1], NULL, 10));

   if (argc < nrfiles + 2) { //7
      std::cerr << "NRFILES < NO-OF-ARGUMENTS" << std::endl << std::endl;
      std::cerr << invocation << std::endl;
      exit(0);
   }

   set <string> rtedfiles;
   string abs_path="";

   for (int i=2; i< nrfiles + 2; ++i)
   {
     string filename = argv[i];
     int    pos=filename.rfind("/");

     if (pos>0 && pos!=(int)string::npos) {
        abs_path = filename.substr(0,pos+1);
     }

     rtedfiles.insert( system_complete( filename ).file_string() );
     if (RTEDDEBUG())
        cerr << i << ": >>>>> Found filename : " << filename << endl;
   }
   // files are pushed on through bash script
   //sort(rtedfiles.begin(), rtedfiles.end(),greater<string>());
   if (RTEDDEBUG())
      cerr << " >>>>>>>>>>>>>>>>>>>> NR OF FILES :: " << rtedfiles.size() << endl;

   // move arguments one to left
   //~ for (int i=2;i<argc;++i) {
      //~ argv[i-1]=argv[i];
   //~ }
   //~ argc=argc-1;
   //~ for (int i=0; i< argc-1; ++i) {
      //~ cout << i << " : " << argv[i] << endl;
   //~ }

   for (int i=1; i< argc; ++i) {
      cout << i << " : " << argv[i] << endl;
   }

   if (RTEDDEBUG())
      cerr << "Running RTED in :" << abs_path << endl;

   runtimeCheck(argc-1, argv+1, rtedfiles);
   return 0;
}

#endif
