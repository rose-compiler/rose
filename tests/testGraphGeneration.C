// Example ROSE Translator reads input program and tests AST and WholeAST graph generation.
#include "rose.h"

int main( int argc, char * argv[] )
   {
  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

  // Output an optional graph of the AST (just the tree, when active)
     generateDOT ( *project );

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);

  // regenerate the source code and call the vendor 
  // compiler, only backend error code is reported.
     return backend(project);
   }
