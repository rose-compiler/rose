// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"
#include <sageDoxygen.h>
#include <list>
#include <commandline_processing.h>
#include <string>

using namespace std;

int main( int argc, char * argv[] ) 
   {
     CommandlineProcessing::addCppSourceFileSuffix("docs");
     CommandlineProcessing::addCppSourceFileSuffix("h");

  // Build the AST used by ROSE
     SgProject* sageProject = frontend(argc,argv);

     Doxygen::annotate(sageProject);
     map<string, DoxygenFile *> *files = Doxygen::getFileList(sageProject);
     for (map<string, DoxygenFile *>::iterator i = files->begin(); i != files->end(); ++i) {
          i->second->group("Special functions");
          i->second->group("New function group 1");
          i->second->group("New function group 2");
     }

  // Generate source code from AST and call the vendor's compiler
     sageProject->unparse();
   }

