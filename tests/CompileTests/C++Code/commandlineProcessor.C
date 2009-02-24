// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure
#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <iomanip>

#include "AstTests.h"

#include <algorithm>

#include "sageCommonSourceHeader.h"

extern an_il_header il_header;

int
main ( int argc, char* argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

#if 1
     list<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("Preprocessor (before): argv = \n%s \n",StringUtility::listToString(l).c_str());

  // testing removeArgs
     CommandlineProcessing::removeArgs (argc,argv,"-edg:");
     CommandlineProcessing::removeArgs (argc,argv,"--edg:");
     CommandlineProcessing::removeArgsWithParameters (argc,argv,"-edg_parameter:");
     CommandlineProcessing::removeArgsWithParameters (argc,argv,"--edg_parameter:");

     printf ("argc = %d \n",argc);
     l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("l.size() = %d \n",l.size());
     printf ("Preprocessor (after): argv = \n%s \n",StringUtility::listToString(l).c_str());

  // printf ("Exiting in main! \n");
  // ROSE_ASSERT(1 == 2);
#endif

#if 0
     int modifiedArgc    = 0;
     char** modifiedArgv = NULL;

  // resets modifiedArgc and allocates memory to modifiedArgv
  // list<string> edgOptionWithNameParameterList = 
  //      CommandlineProcessing::generateOptionWithNameParameterList (argc, argv,"-edg_parameter:",modifiedArgc,modifiedArgv);

  // resets modifiedArgc and allocates memory to modifiedArgv
     list<string> edgOptionWithNumberParameterList =
          CommandlineProcessing::generateOptionWithNameParameterList (argc, argv,"-edg_parameter:",modifiedArgc,modifiedArgv);

     l = CommandlineProcessing::generateArgListFromArgcArgv (modifiedArgc,modifiedArgv);
     printf ("Preprocessor (after): argv = \n%s \n",StringUtility::listToString(l).c_str());

  // resets modifiedArgc to zero and releases memory from modifiedArgv (resets modifiedArgv to NULL)
     CommandlineProcessing::releaseArgListMemory(modifiedArgc,modifiedArgv);
#endif

#if 0
     if ( CommandlineProcessing::isOption(argc,argv,"-rose:","(h|help)",true) ||
          CommandlineProcessing::isOption(argc,argv,"-", "(h|help)",true) ||
          CommandlineProcessing::isOption(argc,argv,"--","(h|help)",true) )
        {
          printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
          ROSE::usage(0);
          exit(0);
        }

     l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("Preprocessor (after): argv = \n%s \n",StringUtility::listToString(l).c_str());

     printf ("Exiting after initial command line processing \n");
     ROSE_ABORT();
#endif

#if 0
     string stringParameter;
     if ( CommandlineProcessing::isOptionWithParameter(argc,argv,"-rose:","(o|output)",stringParameter,true) )
        {
          printf ("-rose:output %s \n",stringParameter.c_str());
       // Make our own copy of the filename string
          int length = stringParameter.length();
          char* p_unparse_output_filename = (char*) new char[length+1];
          ROSE_ASSERT (p_unparse_output_filename != NULL);
          stringParameter.copy(p_unparse_output_filename,length,0);
          p_unparse_output_filename[length] = '\0';
          printf ("p_unparse_output_filename = %s \n",p_unparse_output_filename);
        }
       else
        {
          printf ("-rose:output not set! \n");
        }
#endif


#if 0
     int modifiedArgc    = 0;
     char** modifiedArgv = NULL;

  // resets modifiedArgc and allocates memory to modifiedArgv
     list<string> edgOptionList = CommandlineProcessing::generateOptionList (argc, argv,"-edg:",modifiedArgc,modifiedArgv);

  // resets modifiedArgc to zero and releases memory from modifiedArgv (resets modifiedArgv to NULL)
     CommandlineProcessing::releaseArgListMemory(modifiedArgc,modifiedArgv);

     if ( CommandlineProcessing::isOption(argc,argv,"-","help",true) )
        {
          printf ("Option -help found! \n");
        }

     if ( CommandlineProcessing::isOption(argc,argv,"--","help",true) )
        {
          printf ("Option --help found! \n");
        }

     int integerParameter;
     if ( CommandlineProcessing::isOptionWithParameter(argc,argv,"-edg:","test",integerParameter,true) )
        {
          printf ("Option (integer parameter) -test %d found! \n",integerParameter);
        }

     string stringParameter;
     if ( CommandlineProcessing::isOptionWithParameter(argc,argv,"-edg:","test",stringParameter,true) )
        {
          printf ("Option (string parameter) -test %s found! \n",stringParameter.c_str());
        }
#endif

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

#if 0
  // See if we can access the EDG AST directly
     ROSE_ASSERT (il_header.primary_source_file != NULL);
     ROSE_ASSERT (il_header.primary_source_file->file_name != NULL);
     printf ("##### il_header.primary_source_file->file_name = %s \n",il_header.primary_source_file->file_name);
#endif

  // DQ (2/6/2004): These tests fail in Coco for test2004_14.C
  // AstTests::runAllTests(const_cast<SgProject*>(project));

  // printf ("Generate the pdf output of the SAGE III AST \n");
  // generatePDF ( project );

     printf ("Generate the DOT output of the SAGE III AST \n");
     generateDOT ( *project );

     return backend(project);

  // alternative form
  // return backend(frontend(argc,argv));
   }











