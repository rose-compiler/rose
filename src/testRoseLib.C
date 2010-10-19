// Example ROSE Preprocessor
// used for testing ROSE infrastructure
#include "sage3basic.h"
#include "rose.h"

// This trivial example ROSE translator can be considered to 
// be user code. Make sure that autoconf macros are not defined 
// in user code.  ROSE must be able to be built as a library
// where an application may use its own definition for autoconf
// macros (e.g. PACKAGE_BUGREPORT) so we cannot be defining it
// (and many other such macros).
#ifdef PACKAGE_BUGREPORT
   #error "PACKAGE_BUGREPORT should not be defined in user code (rose_config.h included by mistake)."
#endif

int
main( int argc, char * argv[] )
   {
     RoseTestTranslator test;

  // returns an error if any of the front-end, mid-end, or back-end fails
     return test.translate(argc,argv);
   }

/* 
alternative usage:
int main( int argc, char * argv[] )
   {
     SgProject* sageProject = frontend(argc,argv);
     AstTests::runAllTests(sageProject);
     return backend(sageProject); // only backend error code is reported
   }
*/
