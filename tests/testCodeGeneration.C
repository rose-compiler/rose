// Example ROSE Translator used for testing ROSE infrastructure
#include "rose.h"

int main( int argc, char * argv[] )
   {
  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

  // regenerate the source code and call the vendor compiler, only backend error code is reported.
     return backendGeneratesSourceCodeButCompilesUsingOriginalInputFile(project);
   }
