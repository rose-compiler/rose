// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

using namespace std;

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     Rose_STL_Container<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("Preprocessor (before): argv = \n%s \n",StringUtility::listToString(l).c_str());

  // Remove certain sorts of options from the command line
     CommandlineProcessing::removeArgs (l,"-edg:");
     CommandlineProcessing::removeArgs (l,"--edg:");
     CommandlineProcessing::removeArgsWithParameters (l,"-edg_parameter:");
     CommandlineProcessing::removeArgsWithParameters (l,"--edg_parameter:");

  // Add a test for a custom command line option
     int integerOptionForVerbose = 0;
     if ( CommandlineProcessing::isOptionWithParameter(l,"-myTranslator:","(v|verbose)",integerOptionForVerbose,true) )
        {
          printf ("Turning on my translator's verbose mode (set to %d) \n",integerOptionForVerbose);
        }

  // Adding a new command line parameter (for mechanisms in ROSE that take command lines)

     // printf ("argc = %zu \n",l.size());
     // l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("l.size() = %zu \n",l.size());
     printf ("Preprocessor (after): argv = \n%s \n",StringUtility::listToString(l).c_str());

  // SgProject* project = frontend(argc,argv);
  // ROSE_ASSERT (project != NULL);
  // Generate the source code and compile using the vendor's compiler
  // return backend(project);

  // Build the AST, generate the source code and call the backend compiler ... 
     frontend(l);
     return 0;
   }
