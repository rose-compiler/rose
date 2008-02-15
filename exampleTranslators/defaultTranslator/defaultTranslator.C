// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.

#include "rose.h"

// Main Function for default example ROSE Preprocessor
// This is an example of a preprocessor that can be built with ROSE
// This example can be used to test the ROSE infrastructure
int
main ( int argc, char** argv )
{
  CppToCppTranslator c;
  return c.translate(argc,argv);
    
  //alternative:
  /*
    return backend(frontend(argc,argv));
  */
}













