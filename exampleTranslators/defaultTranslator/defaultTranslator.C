// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.

#include "rose.h"

// Main Function for default example ROSE Preprocessor
// This is an example of a preprocessor that can be built with ROSE
// This example can be used to test the ROSE infrastructure
int
main ( int argc, char** argv )
{
#if 0
  CppToCppTranslator c;
  return c.translate(argc,argv);
#else    
  //alternative:
  /*
    return backend(frontend(argc,argv));
  */
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Run internal consistency tests on AST
     AstTests::runAllTests(project);

  // Insert your own manipulation of the AST here...

  // Generate source code from AST and call the vendor's compiler
     return backend(project);

#endif
}













