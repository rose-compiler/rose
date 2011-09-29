#include "rose.h"
#include "wholeAST.h"
#include <vector>
using namespace std;

// Large arbitrary graphs are a problem to layout using GraphViz (and generally)
// So these macros define limits where it is whole graph mechanism not only takes 
// too long, but is too complex to even be helpful.
#define MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH 8000

// Graphs as trees can be visualized at around 100K nodes, but they are not useful 
// at that size.
#define MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_TREE_GRAPH 16000

int main( int argc, char* argv[] )
   {
     // Liao, 10/23/2009, enable command options to change internal behaviors of the dot graph generator
     vector<string>  argvList (argv, argv+ argc);
//     generateGraphOfAST_initFilters(argvList);
     CustomMemoryPoolDOTGeneration::s_Filter_Flags* filter_flags = new CustomMemoryPoolDOTGeneration::s_Filter_Flags(argvList);

     //SgProject* project = frontend(argc,argv);
     SgProject* project = frontend(argvList);
     ROSE_ASSERT (project != NULL);

  // Generate the filename to be used as a base for generated dot files (graphs)
  // this will be a concatenation of the source file names were more than one is 
  // specified on the command line.
     string filename = SageInterface::generateProjectName(project);

     int numberOfASTnodesBeforeMerge = numberOfNodes();
     printf ("numberOfASTnodesBeforeMerge = %d MAX_NUMBER_OF_IR_NODES_TO_GRAPH = %d MAX_NUMBER_OF_IR_NODES_TO_GRAPH = %d \n",
          numberOfASTnodesBeforeMerge,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_TREE_GRAPH);
     if (numberOfASTnodesBeforeMerge < MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH)
        {
       // default generated graphof whole AST excludes front-end specific IR node (e.g. builtin functions)
          generateWholeGraphOfAST(filename+"_WholeAST", filter_flags);

       // The call to generateWholeGraphOfAST is the same as the following:
       // set<SgNode*> skippedNodeSet = getSetOfFrontendSpecificNodes();
       // generateWholeGraphOfAST(filename+"_WholeAST_skipped_nodes",skippedNodeSet);

       // generate the AST whole graph by providing an empty set to a similar function
          set<SgNode*> emptySet;
          generateWholeGraphOfAST(filename+"_WholeAST_with_builtin_functions",emptySet);
        }
       else
        {
          printf ("AST too large to output as a graph (non-tree) and layout using GraphViz, so output skipped \n");
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
          printf ("AST too large to output as a tree and layout using GraphViz, so output skipped\n");
        }

     return 0;
   }
