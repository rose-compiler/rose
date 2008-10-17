#include "rose.h"
#include <sageDoxygen.h>
#include <list>
#include <commandline_processing.h>
#include <string>

using namespace std;

int main( int argc, char * argv[] ) 
   {
     vector<string> argvList(argv, argv + argc);

     CommandlineProcessing::addCppSourceFileSuffix("docs");
     CommandlineProcessing::addCppSourceFileSuffix("h");

     Doxygen::parseCommandLine(argvList);

  // Build the AST used by ROSE
     SgProject* sageProject = frontend(argvList);

     Doxygen::annotate(sageProject);

     Doxygen::correctAllComments(sageProject);

  // Generate source code from AST
     Doxygen::unparse(sageProject);
     sageProject->unparse();
   }

