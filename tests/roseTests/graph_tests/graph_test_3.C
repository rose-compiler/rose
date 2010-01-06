// Test code to demonstrate the construction of graphs using the new ROSE 
// Graph IR nodes (only the API is new, the IR nodes have existed for a while).

#include "rose.h"

// Required for Boost Spanning Tree support.
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>

using namespace std;

using namespace boost;

#ifndef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
#warning "This file will not work with the new API, must use the backward compatability mode."
#endif


int main()
   {
// tps (6 May 2009) : This test uses old interfaces that I am commenting out.
// need to get everything else working first and then come back to this
// #if 1

  // This is the ROSE Graph (using the new IR nodes)
     SgGraph G("demo graph");

#ifdef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
     typedef adjacency_list < vecS, vecS, undirectedS, no_property, property < edge_weight_t, int > > Graph;

     typedef graph_traits < Graph >::edge_descriptor Edge;

  // DQ (4/29/2009): We might need this in the future, but not now!
  // typedef graph_traits < Graph >::vertex_descriptor Vertex;

     typedef std::pair<int, int> E;


     printf ("Adding nodes \n");
     SgGraphNode* node_a = G.addNode("A");
     SgGraphNode* node_b = G.addNode("B");

     printf ("Adding edges \n");
     G.addEdge(node_a,node_b,"edge ab");

#if 1
     const int SIZE = 3;
     SgGraphNode* nodes[SIZE];

     printf ("Adding nodes SIZE = %d \n",SIZE);
     for (int i=0; i < SIZE; i++)
        {
          nodes[i] = new SgGraphNode();
          ROSE_ASSERT(nodes[i] != NULL);

       // We need to add the ndes to the graph
          printf ("Adding node i = %d node->get_index() = %u \n",i,nodes[i]->get_index());
          G.addNode(nodes[i]);
        }

     int numberOfNodes = SIZE;

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

     printf ("The MST algorithm only needs information about the edges, not the actual nodes of the graph. \n");

  // The MST algorithm only needs information about the edges, not the actual nodes of the graph.
     vector<E> edge_vector;
     vector<int> edge_weights;
     SgGraphEdgeList::iterator edgeIt = G.get_edges()->get_edges().begin();
     while(edgeIt != G.get_edges()->get_edges().end())
        {
       // We want the edges to have an index ordering (could be computed or saved, for now we will save it explicitly).
          int i = edgeIt->second->get_node_A()->get_index();
          int j = edgeIt->second->get_node_B()->get_index();

       // Generate the edge vector for MST (required for many BGL algorithms)
          edge_vector.push_back(E(i,j));

       // Use constant edge weight for now (I think that the edge weights are required by the MST algorithm)
          edge_weights.push_back(1);

          edgeIt++;
        }

     Graph rose_BGL_graph(edge_vector.begin(), edge_vector.end(), edge_weights.begin(), numberOfNodes);
  // property_map < Graph, edge_weight_t >::type weight = get(edge_weight, g);
     std::vector < Edge > rose_spanning_tree;

#if 1
     printf ("Computing the kruskal_minimum_spanning_tree() \n");
     kruskal_minimum_spanning_tree(rose_BGL_graph, std::back_inserter(rose_spanning_tree));
     printf ("DONE: Computing the kruskal_minimum_spanning_tree() \n");

     printf ("rose_spanning_tree.size() = %zu \n",rose_spanning_tree.size());
#endif

#endif

     int num_nodes = 5;
     E edge_array[] = { E(0, 2), E(1, 3), E(1, 4), E(2, 1), E(2, 3), E(3, 4), E(4, 0), E(4, 1) };
     int weights[] = { 1, 1, 2, 7, 3, 1, 1, 1 };



  // This is just the number of entries in the edge_array
     std::size_t num_edges = sizeof(edge_array) / sizeof(E);

     Graph g(edge_array, edge_array + num_edges, weights, num_nodes);
  // Graph g(edge_array, edge_array + num_edges, num_nodes);

  // property_map < Graph, edge_weight_t >::type weight = get(edge_weight, g);

     std::vector < Edge > spanning_tree;
#if 0
     kruskal_minimum_spanning_tree(g, std::back_inserter(spanning_tree));
#endif



// endif for ifdef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
#endif

// #endif

     return 0;
   }



