// This functionality has been moved into librose. See rose::EditDistance::TreeEditDistance. [Robb P. Matzke 2014-09-22]





#include "rose.h"

using namespace std;

#include "treeEditDistance.h"
#include "sequenceGeneration.h"

// Boost required header files for shortest path analysis
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;

typedef adjacency_list < listS, vecS, directedS,no_property, property < edge_weight_t, int > > graph_t;
typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;

void
outputBoostShortestPath( graph_t & g, int edgeMatrixSize_X, std::vector<vertex_descriptor> & p, std::vector<int> & d )
   {
     std::cout << "distances and parents:" << std::endl;
     graph_traits < graph_t >::vertex_iterator vi, vend;

     printf ("graph size (num_vertices(g)) = %zu edgeMatrixSize_X = %d \n",num_vertices(g),edgeMatrixSize_X);
     for (tie(vi, vend) = vertices(g); vi != vend; ++vi)
        {
       // printf ("*vi = %d \n",*vi);
       // string nodeName   = StringUtility::numberToString(*vi/n2) + ":" + StringUtility::numberToString(*vi % n2);
       // string parentName = StringUtility::numberToString(p[*vi]/n2) + ":" + StringUtility::numberToString(p[*vi] % n2);
          string nodeName   = StringUtility::numberToString(*vi);
          string parentName = StringUtility::numberToString(p[*vi]);

          std::cout << "distance(" << nodeName << ") = " << d[*vi] << ", parent(" << nodeName << ") = " << parentName << std::endl;
        }
     std::cout << std::endl;

  // It is not required to output the nodes in the graph, but the filename of the dot file can't contain a "-"!
     std::ofstream dot_file("dijkstra.dot");

#if 1
     dot_file << "digraph D {\n";
     dot_file << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";
#else
  // This builds too simple of a graph to support anything large
     dot_file << "digraph D {\n"
              << " rankdir=LR\n"
              << " size=\"4,3\"\n"
              << " ratio=\"fill\"\n"
              << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";
#endif

     property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);

#if 0
  // It is not required to output the nodes in the graph, but the filename of the dot file can't contain a "-"!
     dot_file << "// Nodes \n";

     graph_traits < graph_t >::vertex_iterator ni, ni_end;
     for (tie(ni, ni_end) = vertices(g); ni != ni_end; ++ni)
        {
          graph_traits < graph_t >::vertex_descriptor n = *ni;

       // string nodeName = StringUtility::numberToString(u/n2) + ":" + StringUtility::numberToString(u % n2);
          string nodeName_n = StringUtility::numberToString(n);

          printf ("n = %zu nodeName_n = %s \n",n,nodeName_n.c_str());
          dot_file << "\"" << nodeName_n << "\"[label=\"" << nodeName_n << "\"";
               dot_file << ", color=\"red\"";
               dot_file << "];\n";
        }
#endif

#if 1
     dot_file << "// Edges \n";
     graph_traits < graph_t >::edge_iterator ei, ei_end;
     for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        {
          graph_traits < graph_t >::edge_descriptor e = *ei;
          graph_traits < graph_t >::vertex_descriptor u = source(e, g), v = target(e, g);

       // string nodeName = StringUtility::numberToString(u/n2) + ":" + StringUtility::numberToString(u % n2);
          string nodeName_u = StringUtility::numberToString(u);
          string nodeName_v = StringUtility::numberToString(v);

#if 0
          printf ("u = %zu v = %zu nodeName_u = %s nodeName_v = %s \n",u,v,nodeName_u.c_str(),nodeName_v.c_str());
          if (p[v] == u)
             {
               dot_file << nodeName_u << " -> " << nodeName_v
                     // << "[label=\"" << get(weightmap, e) << "\"";
                        << "[label=\"" << "1" << "\"";
               dot_file << ", color=\"green\"";
               dot_file << "] ";
             }
#else
       // Original example code
          dot_file << nodeName_u << " -> " << nodeName_v
                   << "[label=\"" << get(weightmap, e) << "\"";
          if (p[v] == u)
               dot_file << ", color=\"violet\"";
            else
               dot_file << ", color=\"black\"";
          dot_file << "]";
#endif
        }
#endif
     dot_file << "}";
   }


// void tree_edit_graph( SgNode* T1, SgNode* T2, EdgeGraph & G )
void
tree_edit_distance( SgNode* T1, SgNode* T2 )
   {
     printf ("Inside of tree_edit_graph() \n");

     map<SgNode*,int> nodeOrder1, nodeOrder2, nodeDepth1, nodeDepth2;

     vector< SgNode* > traversalTrace1;
     vector< SgNode* > traversalTrace2;

  // Generate a preorder traversal trace from depth 0 (root) to -1 (leaves).
     generateTraversalListSupport ( T1, traversalTrace1, nodeOrder1, nodeDepth1, 0, -1 );
     generateTraversalListSupport ( T2, traversalTrace2, nodeOrder2, nodeDepth2, 0, -1 );

     printf ("traversalTrace1.size() = %zu nodeOrder1.size() = %zu nodeDepth1.size() = %zu \n",traversalTrace1.size(),nodeOrder1.size(),nodeDepth1.size());
     printf ("traversalTrace2.size() = %zu nodeOrder2.size() = %zu nodeDepth2.size() = %zu \n",traversalTrace2.size(),nodeOrder1.size(),nodeDepth1.size());

     ROSE_ASSERT(nodeDepth1.empty() == false);
     ROSE_ASSERT(nodeDepth2.empty() == false);

     int n1 = traversalTrace1.size();
     int n2 = traversalTrace2.size();

     printf ("n1 = %d n2 = %d \n",n1,n2);

  // Arrays to store the depths of each subtree
     vector<int> d1(n1+1);
     vector<int> d2(n2+1);

     printf ("Iterate over traversalTrace1 \n");

  // forall_nodes(v,T1) d1[num1[v]] = depth1[v];
  // forall_nodes(w,T2) d2[num2[w]] = depth2[w];

  // int counter = 0;
     d1[0] = 0;
     for (vector<SgNode*>::iterator i = traversalTrace1.begin(); i != traversalTrace1.end(); i++)
        {
       // SgNode* node = *i;
       // printf ("counter = %d node = %p = %s nodeOrder1[node] = %d nodeDepth1[*i] = %d \n",counter,node,node->class_name().c_str(),nodeOrder1[node],nodeDepth1[node]);

          d1[nodeOrder1[*i]+1] = nodeDepth1[*i];

       // counter++;
        }

  // counter = 0;
     d2[0] = 0;
     for (vector<SgNode*>::iterator i = traversalTrace2.begin(); i != traversalTrace2.end(); i++)
        {
       // printf ("counter = %d *i = %p = %s \n",counter,*i,(*i)->class_name().c_str());

          d2[nodeOrder2[*i]+1] = nodeDepth2[*i];

       // counter++;
        }

  // printf ("\n\nEdge Graph:\n");
     string dotGraph = "digraph \"Edge Graph\" { \n";

  // Boost types
     typedef std::pair<int, int> Edge;
     vector<Edge> edge_array;

  // Draw the nodes
     for ( int i = 0; i <= n1; i++ )
        {
          for ( int j = 0; j <= n2; j++ )
             {
            // printf ("%s ",A[ i * (n2 + 1) + j ].c_str());
               int index = i * (n2+1) + j;
               string label = StringUtility::numberToString(index);

            // printf ("i = %d j = %d index = %d label = %s \n",i,j,index,label.c_str());
               string nodeName = label;
               if (i > 0 && j > 0)
                  {
#if 1
                 // Comment out to make the labels simpler (shorter) for fater handling in DOT.
                 // Actually I don't think it makes much difference since the DOT file is large 
                 // and still just as slow to layout and use with zgrviewer.
                    nodeName = traversalTrace1[i-1]->class_name() + "\\n" + traversalTrace2[j-1]->class_name();
#endif
                  }
                 else
                  {
                    if (i > 0)
                       {
                      // nodeName = StringUtility::numberToString(i);
                         nodeName = traversalTrace1[i-1]->class_name();
                       }
                      else
                       {
                         if (j > 0)
                            {
                           // nodeName = StringUtility::numberToString(j);
                              nodeName = traversalTrace2[j-1]->class_name();
                            }
                           else
                            {
                              ROSE_ASSERT(i == 0 && j == 0);
                              nodeName = "ROOT: SgProject";
                            }
                       }
                  }
               dotGraph += "\"" + label + "\"[label=\"" + nodeName + "\" ];\n";
             }
        }

  // Note that the graph is (n1+1) X (n2+1) in size!
  // Draw the edges (use vertical edges with color matching background to get layout in matrix.
  // Specify horizontal edges to be drawn with "constraint=false" so that the layout will remain as a matrix.
     for ( int i = 0; i <= n1; i++ )
        {
          for ( int j = 0; j <= n2; j++ )
             {
               int currentNodeId = i*(n2+1)+j;
               int rightNodeId   = i*(n2+1)+(j+1);
               int downNodeId    = (i+1)*(n2+1)+j;

               string current_label = StringUtility::numberToString(currentNodeId);
               string right_label   = StringUtility::numberToString(rightNodeId);
               string down_label    = StringUtility::numberToString(downNodeId);

               if (i < n1)
                  {
                    dotGraph += "\"" + current_label + "\" -> \"" + down_label + "\"[ dir=none color=white ];\n";
                  }

               if (j < n2)
                  {
                    dotGraph += "\"" + current_label + "\" -> \"" + right_label + "\"[ dir=none color=white constraint=false ];\n";
                  }
             }
        }

  // Interpretation of edits on boundaries
  // Add default edge conditions: RED == DELETE
     for ( int i = 0; i < n1; i++ )
        {
          int currentNodeId = i*(n2+1)+n2;
          int downNodeId    = (i+1)*(n2+1)+n2;

          string current_label = StringUtility::numberToString(currentNodeId);
          string down_label    = StringUtility::numberToString(downNodeId);

          dotGraph += "\"" + current_label + "\" -> \"" + down_label + "\"[ dir=none color=red4 style=\"bold\" constraint=false ];\n";

          edge_array.push_back(Edge(currentNodeId,downNodeId));
        }

  // Add default edge conditions: GREEN == insert
     for ( int j = 0; j < n2; j++ )
        {
          int currentNodeId = n1*(n2+1)+j;
          int rightNodeId   = n1*(n2+1)+(j+1);

          string current_label = StringUtility::numberToString(currentNodeId);
          string right_label   = StringUtility::numberToString(rightNodeId);

          dotGraph += "\"" + current_label + "\" -> \"" + right_label + "\"[ dir=none color=green4 style=\"bold\" constraint=false ];\n";

          edge_array.push_back(Edge(currentNodeId,rightNodeId));
        }
     
     for ( int i = 0; i < n1; i++ )
        {
          for ( int j = 0; j < n2; j++ )
             {
               int currentNodeId  = i*(n2+1)+j;
               int rightNodeId    = i*(n2+1)+(j+1);
               int downNodeId     = (i+1)*(n2+1)+j;
               int diagonalNodeId = (i+1)*(n2+1)+(j+1);

            // RED == DELETE
               if ( d1[i+1] >= d2[j+1] )
                  {
                    string current_label = StringUtility::numberToString(currentNodeId);
                    string down_label    = StringUtility::numberToString(downNodeId);

                    dotGraph += "\"" + current_label + "\" -> \"" + down_label + "\"[ dir=none color=red constraint=false ];\n";

                    edge_array.push_back(Edge(currentNodeId,downNodeId));
                  }

            // BLUE == SUBSTITUTE
               if ( d1[i+1] == d2[j+1] )
                  {
                    string current_label = StringUtility::numberToString(currentNodeId);
                    string diagonal_label = StringUtility::numberToString(diagonalNodeId);

                    dotGraph += "\"" + current_label + "\" -> \"" + diagonal_label + "\"[ dir=none color=blue constraint=false ];\n";

                    edge_array.push_back(Edge(currentNodeId,diagonalNodeId));
                  }

            // GREEN == insert
               if ( d1[i+1] <= d2[j+1] )
                  {
                    string current_label = StringUtility::numberToString(currentNodeId);
                    string right_label   = StringUtility::numberToString(rightNodeId);

                    dotGraph += "\"" + current_label + "\" -> \"" + right_label + "\"[ dir=none color=green constraint=false ];\n";

                    edge_array.push_back(Edge(currentNodeId,rightNodeId));
                  }
             }
        }


  // printf ("Dot graph = \n%s \n",dotGraph.c_str());

  // Build the weights for the edges (later these will not be non-unit values).
     int* weights = new int[edge_array.size()];
     for (size_t i = 0; i < edge_array.size(); i++)
        {
          weights[i] = 1;
        }

#if 0
  // Draw the edges used as input to the shortest path algorithm (supports debugging).
     for (size_t i = 0; i < edge_array.size(); i++)
        {
          string from_label = StringUtility::numberToString(edge_array[i].first);
          string to_label   = StringUtility::numberToString(edge_array[i].second);
          printf ("from_label = %s to_label = %s \n",from_label.c_str(),to_label.c_str());
          dotGraph += "\"" + from_label + "\" -> \"" + to_label + "\"[ color=yellow constraint=false ];\n";
        }
#endif

  // Build the boost graph
     graph_t boostGraph (edge_array.begin(), edge_array.end(), weights, n1*n2);

  // Build the parent array
     std::vector<vertex_descriptor> parentNodes(num_vertices(boostGraph));

  // Build the distance array (distance from source node to all other nodes)
     std::vector<int> distance(num_vertices(boostGraph));

  // Specification of source node (SgProject)
     vertex_descriptor sourceNode = vertex(0, boostGraph);

     dijkstra_shortest_paths(boostGraph, sourceNode, predecessor_map(&parentNodes[0]).distance_map(&distance[0]));

#if 0
     property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, boostGraph);

  // Draw edges on DOT graph:
     dotGraph += "// Edges \n";
     graph_traits < graph_t >::edge_iterator ei, ei_end;
     counter = 0;
     for (tie(ei, ei_end) = edges(boostGraph); ei != ei_end; ++ei)
        {
          graph_traits < graph_t >::edge_descriptor e = *ei;
          graph_traits < graph_t >::vertex_descriptor u = source(e, boostGraph), v = target(e, boostGraph);

       // string nodeName = StringUtility::numberToString(u/n2) + ":" + StringUtility::numberToString(u % n2);
          string nodeName_u = StringUtility::numberToString(u);
          string nodeName_v = StringUtility::numberToString(v);

       // Original example code
          dotGraph += nodeName_u + " -> " + nodeName_v + string("[label=\"") + StringUtility::numberToString(get(weightmap, e)) + "\"";
          if (parentNodes[v] == u)
               dotGraph += ", color=\"violet\" ";
            else
               dotGraph += ", color=\"black\"";

          dotGraph += "constraint=false ]";
          counter++;
        }

     printf ("Edge loop: counter = %d \n",counter);
#endif

  // Get last node in preorder traversal
     int first_node = 0;
     int last_node = ((n1+1)*(n2+1)) - 1;
     printf ("last_node = %d \n",last_node);

     int path_index = last_node;
     while (path_index != first_node)    
        {
          int parentNode = parentNodes[path_index];
          string editOperation;
          if (parentNode == path_index-1)
             {
            // insertion
               editOperation = "insertion";
             }
            else
             {
               if (parentNode == path_index-(n2+1))
                  {
                 // deletion
                    editOperation = "deletion";
                  }
                 else
                  {
                    if (parentNode == path_index-((n2+1) + 1))
                       {
                      // substitution
                         editOperation = "substitution";
                       }
                      else
                       {
                      // error
                         printf ("Error: parent node is not a properly formed path_index = %d parentNode = %d difference = %d \n",path_index,parentNode,path_index-parentNode);
                         ROSE_ASSERT(false);
                       }
                  }
             }

          string from_label = StringUtility::numberToString(parentNodes[path_index]);
          string to_label   = StringUtility::numberToString(path_index);
          printf ("editOperation = %s from_label = %s to_label = %s \n",editOperation.c_str(),from_label.c_str(),to_label.c_str());

       // DQ (1/17/2009): Note that the "penwidth=n" has not effect (for any value of n) (at least within zgrviewer).
       // dotGraph += "\"" + from_label + "\" -> \"" + to_label + "\"[ color=black penwidth=25.0 style=bold constraint=false ];\n";
          dotGraph += "\"" + from_label + "\" -> \"" + to_label + "\"[ color=black style=bold constraint=false ];\n";

          path_index = parentNodes[path_index];
        }

     dotGraph += "}\n";

  // Output the information in the boostGraph and the shortest_path algorithm
  // outputBoostShortestPath( boostGraph , n2, parentNodes, distance );

     FILE* output1 = fopen("output-local.dot", "w");
     assert (output1);
     fprintf(output1,"%s\n", dotGraph.c_str());
     fclose(output1);

     std::cout <<"total cost = " <<distance[num_vertices(boostGraph)-1] <<"\n";

     printf ("Leaving tree_edit_graph() \n");
   }
