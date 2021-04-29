#include "rose.h"

#include "UnparseHeadersTransformVisitorUsingTokens.h"

int main(int argc, char* argv[])
   {
     ROSE_ASSERT(argc > 1);

     SgProject* project = frontend(argc,argv);

     SgProject::set_unparseHeaderFilesDebug(0);

#if 0
     printf ("Setting unparseHeaderFilesDebug: value = %d \n",SgProject::get_unparseHeaderFilesDebug());
#endif

  // AstTests::runAllTests(project);

#if 0
     printf ("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT \n");
     printf ("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT \n");
     printf ("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT \n");
     printf ("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT \n");
     printf ("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT \n");
#endif

#if 1
     UnparseHeadersTransformVisitor transformVisitor;
     transformVisitor.traverse(project, preorder);
#else
     printf ("SKIPPING THE TRANSFORMATIONS \n");
#endif

#if 0
     printf ("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT \n");
     printf ("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT \n");
     printf ("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT \n");
     printf ("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT \n");
     printf ("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT \n");
#endif

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
  // generateDOT ( *project );
     generateDOT_withIncludes ( *project );
#endif

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

     return backend(project);
   }

