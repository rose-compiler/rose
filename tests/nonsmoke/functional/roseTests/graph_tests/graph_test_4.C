// Test code to demonstrate the construction of graphs using the new ROSE 
// Graph IR nodes (only the API is new, the IR nodes have existed for a while).

#include "rose.h"

using namespace std;

#ifndef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
#warning "This file will not work with the new API, must use the backward compatability mode."
#endif

int main( int argc, char * argv[] )
   {
  // tps (May 7th 2009) - commented this out since it uses the old graph interface
  // Dan needs to look at this 
  // #if 1

     vector<string> sourceCommandline = vector<string>(argv, argv + argc);

  // Build the source AST used by ROSE (only a few IR nodes are built)
     SgProject* project = frontend(sourceCommandline);

     printf ("At this point we have generated a source AST: numberOfNodes() = %d \n",numberOfNodes());

  // Run internal consistancy tests on AST
     AstTests::runAllTests(project);

  // This is the ROSE Graph (using the new IR nodes)
     SgGraph* G = new SgGraph("demo graph");

#ifdef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
     SgGraphNodeList* graph_node_list = G->get_nodes();
     ROSE_ASSERT(graph_node_list != NULL);

     printf ("Calling rose_graph_hash_multimap node_map = graph_node_list->get_nodes(); \n");

     printf ("graph_node_list->get_nodes().size() = %zu \n",graph_node_list->get_nodes().size());
     
#if 0
  // Note that it is a runtime error to not use a "&"
     rose_graph_hash_multimap & node_map = graph_node_list->get_nodes();
#else
  // Failing example
     rose_graph_hash_multimap node_map = graph_node_list->get_nodes();
#endif

     printf ("node_map.size() = %zu \n",node_map.size());

     printf ("Adding nodes \n");
     SgGraphNode* node_a = G->addNode("A");
     SgGraphNode* node_b = G->addNode("B");

     printf ("Adding edges \n");
     G->addEdge(node_a,node_b,"edge ab");

#if 1
     const int SIZE = 5;
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
                 // printf ("Adding edge from i=%d to j=%d \n",i,j);
                    G->addEdge(nodes[i],nodes[j]);
                  }
             }
        }

  // Generate the Minimum Spanning Tree using Boost.
  // vector <SgGraph::BoostEdgeDescriptor> spanningTree = G->generateSpanningTree();
     G->generateSpanningTree();
#endif

#if 0
     for (i=0; i < spanningTree.size(); i++)
        {
          int spanningTree[i]->
        }

     std::cout << "Print the edges in the MST:" << std::endl;
     for (std::vector < Edge >::iterator ei = spanning_tree.begin(); ei != spanning_tree.end(); ++ei)
        {
          cout << source(*ei, g) << " <--> " << target(*ei, g)
               << " with weight of " << weight[*ei]
               << endl;
        }

     std::ofstream fout("figs/kruskal-eg.dot");
     fout << "graph A {\n"
          << " rankdir=LR\n"
          << " size=\"3,3\"\n"
          << " ratio=\"filled\"\n"
          << " edge[style=\"bold\"]\n" << " node[shape=\"circle\"]\n";
     graph_traits<Graph>::edge_iterator eiter, eiter_end;
     for (tie(eiter, eiter_end) = edges(g); eiter != eiter_end; ++eiter)
        {
          fout << source(*eiter, g) << " -- " << target(*eiter, g);
          if (std::find(spanning_tree.begin(), spanning_tree.end(), *eiter) != spanning_tree.end())
               fout << "[color=\"black\", label=\"" << get(edge_weight, g, *eiter) << "\"];\n";
            else
               fout << "[color=\"gray\", label=\"" << get(edge_weight, g, *eiter) << "\"];\n";
        }
     fout << "}\n";

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

// endif for ifdef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
#endif

  // #endif

     return 0;
   }
