// Example ROSE Preprocessor
// used for testing ROSE infrastructure
#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <iomanip>

#include "AstTests.h"

int
main( int argc, char * argv[] ) {
  RoseTestTranslator test;
  return test.translate(argc,argv); // returns an error if any of the ends fails
}

/* 
   alternative usage:
   int main( int argc, char * argv[] ) {
      SgProject* sageProject=frontend(argc,argv);
      AstTests::runAllTests(sageProject);
      return backend(sageProject); // only backend error code is reported
   }
*/
