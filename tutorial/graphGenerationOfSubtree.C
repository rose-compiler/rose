// This is a placeholder until I get the correct version of this file (from Andreas?)


// Example ROSE Translator
// used for testing ROSE infrastructure

#include "rose.h"

int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     AstTests::runAllTests(project);
     return backend(project); // only backend error code is reported
   }
