// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"
#include "Diagnostics.h"
using namespace rose::Diagnostics;

int main( int argc, char * argv[] ) 
   {
  // Parse the command line and parse source files to build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

     mprintf("running consistency tests\n");
     AstTests::runAllTests(project);

  // Insert your own manipulation of the AST here...

     mprintf("generating source code from the AST and calling the vendor's compiler\n");
     int status = backend(project);
     mprintf("backend exit status is %d\n", status);
     return status;
   }

