#include "rose.h"

#include "newCallGraph.h"

#define DEBUG_CALLGRAPH 0

// using namespace NewCallGraph;

int
main ( int argc, char* argv[] )
   {
  // This builds the AST.
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

#if 0
  // Output an optional graph of the AST (just the tree, when active)
  // generateDOT ( *project );
     generateDOTforMultipleFile(*project);

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

#if 1
  // Call function representing the Call Graph API.
     int status = NewCallGraph::buildCallGraph(project);
#else
  // Put analysis traversal here!
     int status = 0;
     CallGraph treeTraversal;
#if 0
     treeTraversal.traverseInputFiles ( project, preorder );
#else
     treeTraversal.traverse ( project, preorder );
#endif
#endif

     printf ("Program Terminated Normally! \n");

     return status;
   }

