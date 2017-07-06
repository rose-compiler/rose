// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See Rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Generate a DOT file to use in visualizing the AST graph.
     generateDOT ( *project );

     return 0;
   }

