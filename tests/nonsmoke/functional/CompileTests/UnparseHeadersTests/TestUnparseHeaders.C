#include "rose.h"

#include "UnparseHeadersTransformVisitor.h"

int main(int argc, char* argv[])
   {
     ROSE_ASSERT(argc > 1);

     SgProject* project = frontend(argc,argv);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // AstTests::runAllTests(project);

     UnparseHeadersTransformVisitor transformVisitor;
     transformVisitor.traverse(project, preorder);

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (In TestUnparseHeaders.C main(): ROSE Release Note: turn off output of dot files before committing code) \n");
  // generateDOT ( *project );
     generateDOT_withIncludes ( *project );
#endif

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

#if 0
     printf ("\n\n************************************* \n");
     printf ("************************************* \n");
     printf ("          Calling backend() \n");
     printf ("************************************* \n");
     printf ("************************************* \n");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     return backend(project);
   }

