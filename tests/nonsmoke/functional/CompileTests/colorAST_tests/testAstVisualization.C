#include "rose.h"

using namespace std;

#define MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH 2000
#define MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_TREE_GRAPH 10000

int main( int argc, char* argv[] )
   {
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Generate the filename to be used as a base for generated dot files (graphs)
  // this will be a concatination of the source file names were more than one is 
  // specified on the command line.
     string filename = SageInterface::generateProjectName(project);

     int numberOfASTnodesBeforeMerge = numberOfNodes();
     printf ("numberOfASTnodesBeforeMerge = %d MAX_NUMBER_OF_IR_NODES_TO_GRAPH = %d MAX_NUMBER_OF_IR_NODES_TO_GRAPH = %d \n",
          numberOfASTnodesBeforeMerge,
          MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,
          MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_TREE_GRAPH);
     if (numberOfASTnodesBeforeMerge < MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH)
        {
       // default generated graphof whole AST excludes front-end specific IR node (e.g. builtin functions)
          generateWholeGraphOfAST(filename+"_WholeAST");

       // generate the AST whole graph by providing an empty set to a similar function
          set<SgNode*> emptySet;
          generateWholeGraphOfAST(filename+"_WholeAST_with_builtin_functions",emptySet);

          set<SgNode*> skippedNodeSet = SageInterface::getFrontendSpecificNodes();
          generateWholeGraphOfAST(filename+"_WholeAST_skipped_nodes",skippedNodeSet);
        }
       else
        {
          printf ("graph too large to output (and layout using GraphViz, so output skipped\n");
        }

  // Alternative approach to output the AST as a graph
     if (numberOfASTnodesBeforeMerge < MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_TREE_GRAPH)
        {
       // graph the AST (now the whole graph, but just as a tree)
          generateGraphOfAST(project,filename+"_AST");
          set<SgNode*> emptySet;
          generateGraphOfAST(project,filename+"_AST_with_builtin_functions",emptySet);
        }
       else
        {
          printf ("graph too large to output (and layout using GraphViz, so output skipped\n");
        }

#if 1
  // test statistics
  // AstNodeStatistics stat;
     if (project->get_verbose() > 1)
        {
       // cout << stat.toString(project);
          cout << AstNodeStatistics::traversalStatistics(project);
          cout << AstNodeStatistics::IRnodeUsageStatistics();
        }
#endif

  // DQ (7/7/2005): Only output the performance report if verbose is set (greater than zero)
     if (project->get_verbose() > 0)
        {
       // Output any saved performance data (see ROSE/src/astDiagnostics/AstPerformance.h)
          AstPerformance::generateReport();
        }

  // return backend(project);
     return 0;
   }
