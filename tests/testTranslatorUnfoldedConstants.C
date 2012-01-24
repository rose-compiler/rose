// Example ROSE Translator used for testing ROSE infrastructure

// This translator does NO constant folding in the frontend (same as default behavior).

#include "rose.h"

int main( int argc, char * argv[] )
   {
  // Generate the ROSE AST.
     bool frontendConstantFolding = false;
     SgProject* project = frontend(argc,argv,frontendConstantFolding);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

  // regenerate the source code and call the vendor 
  // compiler, only backend error code is reported.
     return backend(project);
   }
