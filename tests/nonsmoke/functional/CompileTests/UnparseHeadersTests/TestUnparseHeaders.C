#include "rose.h"

#include "UnparseHeadersTransformVisitor.h"

int main(int argc, char* argv[])
   {
     ROSE_ASSERT(argc > 1);

     SgProject* project = frontend(argc,argv);
    
  // AstTests::runAllTests(project);

     UnparseHeadersTransformVisitor transformVisitor;
     transformVisitor.traverse(project, preorder);

#if 1
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
  // generateDOT ( *project );
     generateDOT_withIncludes ( *project );
#endif

#if 1
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

     return backend(project);
   }

