// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.

#include "rose.h"

int
main( int argc, char* argv[] ) {
  // DQ (4/6/2017): This will not fail if we skip calling ROSE_INITIALIZE (but
  // any warning message using the message looging feature in ROSE will fail).
  ROSE_INITIALIZE;

  CppToDotTranslator c;
  return c.translate(argc,argv);
}  
