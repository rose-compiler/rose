// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Build the DOT file to visualize the AST with attributes (types, symbols, etc.).
  // To protect against building graphs that are too large an option is
  // provided to bound the number of IR nodes for which a graph will be 
  // generated.  The layout of larger graphs is prohibitively expensive.
     const int MAX_NUMBER_OF_IR_NODES = 2000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES);

     return 0;
   }

