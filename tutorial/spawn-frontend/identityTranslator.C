// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"
#include "spawnFrontend.h"

int main( int argc, char * argv[] ) 
   {
  // Initialize and check compatibility. See Rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* sageProject = spawnFrontend(argc,argv);

  // Insert your own manipulation of the AST here...

  // Generate source code from AST and call the vendor's compiler
     // memoryUsageStatistics();
     return backend(sageProject);
   }

