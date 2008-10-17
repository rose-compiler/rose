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

     vector<string> newArgv = argvList;
     newArgv.insert(newArgv.begin() + 1, "-rose:collectAllCommentsAndDirectives");

  // Build the AST used by ROSE
     SgProject* sageProject = frontend(newArgv);

     Doxygen::annotate(sageProject);
     Doxygen::lint(sageProject);
   }


