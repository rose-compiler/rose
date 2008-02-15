// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.

#include "rose.h"

int
main( int argc, char* argv[] ) {
  CppToDotTranslator c;
  return c.translate(argc,argv);
}  
