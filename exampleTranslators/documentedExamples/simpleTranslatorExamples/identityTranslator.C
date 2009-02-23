// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"

int main( int argc, char * argv[] ) 
   {
  // Build the AST used by ROSE
     SgProject* sageProject = frontend(argc,argv);

  // Run internal consistency tests on AST
     AstTests::runAllTests(sageProject);

  // Insert your own manipulation of the AST here...

  // Generate source code from AST and call the vendor's compiler
     return backend(sageProject);
   }

