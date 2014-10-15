// Example ROSE Preprocessor used for testing ROSE infrastructure
// This trivial example ROSE translator can be considered to be user code.

// User code typically includes "rose.h", not "sage3basic.h". Doing so
// here is fine, in fact, recommended since this is emulating user behavior,
// even though the NoRoseIncludes policy complains about it.
#include "rose.h"                                       // POLICY_OK


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
