// Example ROSE Translator reads input program and tests AST and WholeAST graph generation.
#include "rose.h"

// Options to generate graphs with and without filering of
// IR nodes can be use to tailor the generated graph output.
// Example graph options are:
//      -rose:dotgraph:expressionFilter 1
//      -rose:dotgraph:fileInfoFilter 0 

int main( int argc, char * argv[] )
   {
  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

  // Output an optional graph of the AST (just the tree, when active)
     generateDOT ( *project );

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");

  // regenerate the source code and call the vendor 
  // compiler, only backend error code is reported.
     return backend(project);
   }

