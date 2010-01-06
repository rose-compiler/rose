// Test code to demonstrate the construction of graphs using the new ROSE 
// Graph IR nodes (and the addition of attributes to SgGraph, SgGraphNode, and SgGraphEdge).

#include "rose.h"

// Added support for generation of Walrus graphs (3D tree visualization).
#include "walrus.h"

using namespace std;

// forward declaration of function
extern void generateWalrusGraph( SgNode* node, string outputFilename );

int
main( int argc, char * argv[] )
   {
     vector<string> sourceCommandline = vector<string>(argv, argv + argc);

  // Build the source AST used by ROSE (only a few IR nodes are built)
     SgProject* project = frontend(sourceCommandline);

     printf ("At this point we have generated a source AST: numberOfNodes() = %d \n",numberOfNodes());

  // Run internal consistancy tests on AST
     AstTests::runAllTests(project);

#if 0
  // This is the ROSE Graph (using the new IR nodes)
     SgGraph* G = new SgGraph("demo graph");

     const int SIZE = 6;
     SgGraphNode* nodes[SIZE];

     printf ("Adding nodes SIZE = %d \n",SIZE);
     for (int i=0; i < SIZE; i++)
        {
          nodes[i] = new SgGraphNode();
          ROSE_ASSERT(nodes[i] != NULL);

       // We need to add the nodes to the graph
       // printf ("Adding node i = %d node->get_index() = %u \n",i,nodes[i]->get_index());
          G->addNode(nodes[i]);
        }

     printf ("Adding edges SIZE*SIZE = %d \n",SIZE*SIZE);
     for (int i=0; i < SIZE; i++)
        {
          for (int j=0; j < SIZE; j++)
             {
               if (i < j)
                  {
                    printf ("Adding edge from index i=%d to j=%d nodes[i]->get_index() = %d nodes[j]->get_index() = %d\n",i,j,nodes[i]->get_index(),nodes[j]->get_index());
                    SgGraphEdge* edge = G->addEdge(nodes[i],nodes[j]);
                  }
             }
        }

     printf ("Adding nodes SIZE = %d \n",SIZE);
     ROSE_ASSERT(SIZE > 4);
     for (int i=0; i < 4; i++)
        {
       // Test the generation of edge sets
          ROSE_ASSERT(nodes[i] != NULL);
          std::set<SgGraphEdge*> edgeSet = G->computeEdgeSet(nodes[i]);
          printf ("edgeSet size = %zu \n",edgeSet.size());
        }

  // Generate the Minimum Spanning Tree using Boost.
     G->generateSpanningTree();

  // Computed the memory footprint of the graph.
     size_t memoryUsage = G->memory_usage();

#endif

#if 1
  // This function generates a file for use with Walrus graph visualuzation 
  // tool for 3D graphs (note that the graphs must have a spanning tree).
  // WalrusGraph::generateWalrusGraph(project,"/home/dquinlan/test.graph");
     WalrusGraph::generateWalrusGraph(project,"test.graph");
#endif

#if 1
     printf ("Generate the dot output of the SAGE III AST \n");
     generateDOT ( *project );
     printf ("DONE: Generate the dot output of the SAGE III AST \n");
#endif

#if 1
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     printf ("Generate whole AST graph if small enough \n");
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
     printf ("DONE: Generate whole AST graph if small enough \n");
#endif

     return 0;
   }
