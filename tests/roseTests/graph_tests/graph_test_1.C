// Test code to demonstrate the construction of graphs using the new ROSE 
// Graph IR nodes (only the API is new, the IR nodes have existed for a while).

#include "rose.h"

using namespace std;

class MyGraph
   {
  // Note: If we generate a spanning tree on a graph (O(n) complexity), then we could use the 
  // local neighbor lists from the spanning tree to do AST tree traversal to arbitrary graphs
  // (using the dynamic traversal properties of the AST).  Can we make that work???
  // If we wanted to wieght the edges of the control flow we could use that to generate 
  // the minumum spanning tree (using weights based on the inverse of the call frequency).
  // Then with the minimum spanning tree of the control flow we could annotate that with the
  // trace information and define a way to use our tree traversal to do analysis on the traces.
  // The minimum spanning tree would be the hottest trace through the execution of the code.
  // Alternative less minimal spanning trees should be the next hottests paths, we would
  // generally only be interested in a small set of the hottest paths through any one code
  // (taken over the set of expected inputs).  Note also that the hottest path might not be 
  // a feasible path (we might want to restrict ourselves to only spanning trees of feasible 
  // paths).  This then forms the basis of using attribute grammars to evaluate execution traces.

     public:
          SgIncidenceUndirectedGraph graph;


#if 0
          typedef SgIncidenceUndirectedGraph::node_iterator node_iterator;
          typedef SgIncidenceUndirectedGraph::edge_iterator edge_iterator;
#else
#ifdef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
          typedef SgGraphNodeList::iterator node_iterator;
          typedef SgGraphEdgeList::iterator edge_iterator;
#endif
#endif
          MyGraph( string name ) : graph(name)
             {
             }

         SgGraphNode* addNode( const string & name = "");
         SgGraphEdge* addEdge( SgGraphNode* x, SgGraphNode* y, const string & name = "");
   };


SgGraphNode*
MyGraph::addNode( const string & name )
   {
     return graph.addNode(name);
   }

SgGraphEdge*
MyGraph::addEdge( SgGraphNode* x, SgGraphNode* y, const string & name )
   {
     return graph.addEdge(x,y,name);
   }

int main()
   {
  // Since IR nodes can't be built on the stack, this should not work???

     printf ("Sizes of new Graph IR nodes in ROSE: \n");
     printf ("Size of SgGraphNode                     = %zu \n",sizeof(SgGraphNode));
     printf ("Size of SgGraphNodeList                 = %zu \n",sizeof(SgGraphNodeList));

     printf ("Size of SgGraphEdge                     = %zu \n",sizeof(SgGraphEdge));
     printf ("Size of SgDirectedGraphEdge             = %zu \n",sizeof(SgDirectedGraphEdge));
     printf ("Size of SgUndirectedGraphEdge           = %zu \n",sizeof(SgUndirectedGraphEdge));
     printf ("Size of SgGraphEdgeList                 = %zu \n",sizeof(SgGraphEdgeList));

     printf ("Size of SgGraph                         = %zu \n",sizeof(SgGraph));
     printf ("Size of SgIncidenceDirectedGraph        = %zu \n",sizeof(SgIncidenceDirectedGraph));
     printf ("Size of SgIncidenceUndirectedGraph      = %zu \n",sizeof(SgIncidenceUndirectedGraph));
     printf ("Size of SgBidirectionalGraph            = %zu \n",sizeof(SgBidirectionalGraph));
     printf ("Size of SgStringKeyedBidirectionalGraph = %zu \n",sizeof(SgStringKeyedBidirectionalGraph));
     printf ("Size of SgIntKeyedBidirectionalGraph    = %zu \n",sizeof(SgIntKeyedBidirectionalGraph));

     printf ("Size of SgNode                                     = %zu \n",sizeof(SgNode));
     printf ("Size of SgSupport                                  = %zu \n",sizeof(SgSupport));
     printf ("Size of std::string                                = %zu \n",sizeof(std::string));
     printf ("Size of property map: std::map< int, std::string > = %zu \n",sizeof(std::map< int, std::string >));
     printf ("Size of rose_graph_hash_multimap                   = %zu \n",sizeof(rose_graph_hash_multimap));
  // printf ("Size of rose_directed_graph_hash_multimap          = %zu \n",sizeof(rose_directed_graph_hash_multimap));
  // printf ("Size of rose_undirected_graph_hash_multimap        = %zu \n",sizeof(rose_undirected_graph_hash_multimap));
     printf ("Size of rose_graph_node_edge_hash_multimap         = %zu \n",sizeof(rose_graph_node_edge_hash_multimap));

     printf ("Size of AstAttributeMechanism (but we use a pointer) = %zu \n",sizeof(AstAttributeMechanism));


  // Build a graph using the new graph IR nodes in ROSE.
     SgIncidenceUndirectedGraph graph ("demo graph");

  // SgGraphNode* node_a = new SgGraphNode("A");
  // SgGraphNode* node_b = new SgGraphNode("B");

     MyGraph G("demo graph");

  // Reset the size of the internal hash tables.
     G.graph.resize_hash_maps(1000,1000000);

     printf ("Adding nodes \n");
     SgGraphNode* node_a = G.addNode("A");
     SgGraphNode* node_b = G.addNode("B");

     printf ("Adding edges \n");
     G.addEdge(node_a,node_b,"edge ab");

#if 1
     const int SIZE = 1000;
     SgGraphNode* nodes[SIZE];

     printf ("Adding nodes SIZE = %d \n",SIZE);
     for (int i=0; i < SIZE; i++)
        {
          nodes[i] = new SgGraphNode();
          G.graph.addNode(nodes[i]);
        }

     printf ("Adding edges SIZE*SIZE = %d \n",SIZE*SIZE);
     for (int i=0; i < SIZE; i++)
        {
          for (int j=0; j < SIZE; j++)
             {
               if (i < j)
                  {
                 // printf ("Adding edge from i=%d to j=%d \n",i,j);
                    G.addEdge(nodes[i],nodes[j]);
                  }
             }
        }
#endif

  // Computed the memory footprint of the graph.
     size_t memoryUsage = G.graph.memory_usage();
     size_t numberOfGraphNodes = G.graph.numberOfGraphNodes();
     size_t numberOfGraphEdges = G.graph.numberOfGraphEdges();

     printf ("Memory report: numberOfGraphNodes = %zu numberOfGraphEdges = %zu memoryUsage = %zu (bytes) \n",numberOfGraphNodes,numberOfGraphEdges,memoryUsage);

     return 0;
   }
