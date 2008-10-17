// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"
#include <copy_unparser.h>
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

     Doxygen::correctAllComments(sageProject);

  // Generate source code from AST and call the vendor's compiler
     Doxygen::unparse(sageProject);
//     sageProject->unparse();
     CopyUnparser copy(sageProject->get_file(0));
     sageProject->get_file(0).unparse(NULL, &copy);
   }

