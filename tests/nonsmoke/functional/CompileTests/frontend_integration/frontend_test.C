
#include "rose.h"

int main( int argc, char * argv[] )
   {
     SgProject* project = frontend(argc,argv);

     AstTests::runAllTests(project);

  // only backend error code is reported
     return backend(project);
   }
