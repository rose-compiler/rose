// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

int main( int argc, char * argv[] )
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

     generateDOT ( *project );
     return 0;
   }

