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

  // DQ (4/6/2017): This will not fail if we skip calling ROSE_INITIALIZE (but
  // any warning message using the message looging feature in ROSE will fail).
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Insert your own manipulation of the AST here...

  // Generate source code from AST and call the vendor's compiler
     return backend(project);

#endif
}













