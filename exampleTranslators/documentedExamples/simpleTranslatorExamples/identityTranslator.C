// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"

int main( int argc, char * argv[] ) 
   {
  // DQ (4/6/2017): This will not fail if we skip calling ROSE_INITIALIZE (but
  // any warning message using the message looging feature in ROSE will fail).
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Insert your own manipulation of the AST here...

  // Generate source code from AST and call the vendor's compiler
     return backend(project);
   }

