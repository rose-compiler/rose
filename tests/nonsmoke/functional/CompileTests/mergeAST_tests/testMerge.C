#include <rose.h>

// #include "colorTraversal.h"

#define MAX_NUMBER_OF_IR_NODES_TO_GRAPH 8000
#define DISPLAY_INTERNAL_DATA 0

using namespace std;
using namespace Rose;

// Supporting function to process the commandline
void commandLineProcessing (int & argc, char** & argv, bool & skipFrontendSpecificIRnodes)
   {
  // list<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
  // GB (09/26/2007)
     vector<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);

     if (SgProject::get_verbose() > 0)
          printf ("Preprocessor (before): argv = \n%s \n",StringUtility::listToString(l).c_str());

  // bool skipFrontendSpecificIRnodes = false;
  // Add a test for a custom command line option (and remove the options from the commandline; by passing true as last parameter)
     int integerOptionForSupressFrontendCode = 0;
     if ( CommandlineProcessing::isOptionWithParameter(l,"-merge:","(s|suppress_frontend_code)",integerOptionForSupressFrontendCode,true) )
        {
          printf ("Turning on AST merge suppression of graphing fronend-specific IR nodes (set to %d) \n",integerOptionForSupressFrontendCode);
          skipFrontendSpecificIRnodes = true;
        }

  // Adding a new command line parameter (for mechanisms in ROSE that take command lines)

     if (SgProject::get_verbose() > 0)
        {
          printf ("l.size() = %zu \n",(size_t)l.size());
          printf ("Preprocessor (after): argv = \n%s \n",StringUtility::listToString(l).c_str());
        }
   }

int
main ( int argc, char** argv )
   {
  // ****************************************************************************
  // **************************  Command line Processing  ***********************
  // ****************************************************************************
     bool skipFrontendSpecificIRnodes = false;
     commandLineProcessing(argc,argv,skipFrontendSpecificIRnodes);
  // ****************************************************************************

  // SgProject::set_verbose(3);

  // ****************************************************************************
  // **************************      Build the AST    ***************************
  // ****************************************************************************
     SgProject* project = frontend (argc, argv);
     ROSE_ASSERT(project != NULL);

#if 1
  // Run AST tests (takes a while on large ASTs, so we sometime skip this for some phases of development on AST merge)
     if (SgProject::get_verbose() > 0)
          printf ("Running AST tests in main() \n");

     AstTests::runAllTests(project);

     if (SgProject::get_verbose() > 0)
          printf ("Running AST tests in main(): DONE \n");
#else
     printf ("Skipping AST tests in main(): DONE \n");
#endif

#if 0
     printf ("Exiting after running AstTests::runAllTests() \n");
     ROSE_ASSERT(false);
#endif

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
  // DQ (12/22/2019): Call multi-file version (instead of generateDOT() function).
  // generateAstGraph(project, 2000);
  // generateDOT ( *project );
     generateDOTforMultipleFile(*project);
#endif

#if 0
  // DQ (6/14/2007): Added support for simpler function for generation of graph of whole AST.
     const int MAX_NUMBER_OF_IR_NODES = 10000;
     generateAstGraph( project, MAX_NUMBER_OF_IR_NODES, "_beforeMerge" );
#endif

#if 0
  // Output performance information for ROSE plus performance information for AST Merge
     AstPerformance::generateReport();
#endif

     if (SgProject::get_verbose() > 0)
          printf ("Program Terminated Normally! \n");

     return 0;
   }



