// These functions have been separated from Support.code as a way of
// both simplifying the development and also permitting more specialized
// optimizations (file specific compiler flags).

#include "rose.h"

using namespace std;

// DQ (5/3/2009): We might want a version of this function expressed in terms of the node_index and edge_index (integer) values.
// This might not be possible since the rose_graph_integer_edge_hash_multimap is (node index --> SgGraphEdge*).
std::set<SgGraphEdge*>
SgGraph::computeEdgeSet( SgGraphNode* node )
   {
     ROSE_ASSERT(this != NULL);
     ROSE_ASSERT(node != NULL);
#if 0
     typedef std::pair<rose_graph_string_integer_hash_multimap::const_iterator,rose_graph_string_integer_hash_multimap::const_iterator> equal_range_type;
     equal_range_type equal_range_pair = p_string_to_node_index_multimap.equal_range(node->get_name());
     rose_graph_string_integer_hash_multimap::const_iterator lower_bound = equal_range_pair.first;
     rose_graph_string_integer_hash_multimap::const_iterator upper_bound = equal_range_pair.second;
     ROSE_ASSERT(lower_bound != upper_bound);
#endif

     std::set<SgGraphEdge*>  returnSet;

#ifdef ROSE_USE_NEW_GRAPH_NODES
     typedef std::pair<rose_graph_integer_edge_hash_multimap::const_iterator,rose_graph_integer_edge_hash_multimap::const_iterator> equal_range_type;
     equal_range_type equal_range_pair = p_node_index_to_edge_multimap.equal_range(node->get_index());

     for (rose_graph_integer_edge_hash_multimap::const_iterator i = equal_range_pair.first; i != equal_range_pair.second; i++)
        {
       // int edge_index = i->second;
       // SgGraphEdge* edge = p_node_index_to_edge_multimap[edge_index];
          SgGraphEdge* edge = i->second;

          ROSE_ASSERT(edge->get_node_A()->get_index() == node->get_index() || edge->get_node_B()->get_index() == node->get_index());

          printf ("Building set with edge = %d between nodes (%d,%d) \n",edge->get_index(),edge->get_node_A()->get_index(),edge->get_node_B()->get_index());

          returnSet.insert(edge);
        }
#endif

     return returnSet;
   }


std::set<SgGraphNode*>
SgGraph::computeNodeSet( const string & label )
   {
     ROSE_ASSERT(this != NULL);
     std::set<SgGraphNode*>  returnSet;

#ifdef ROSE_USE_NEW_GRAPH_NODES
     std::set<int> nodeIndexSet = computeNodeIndexSet( label );

     std::set<int>::iterator i = nodeIndexSet.begin();
     while (i != nodeIndexSet.end())
        {
          returnSet.insert(p_node_index_to_node_map[*i]);
          i++;
        }
#endif
     return returnSet;
   }

std::set<int>
SgGraph::computeNodeIndexSet( const string & label )
   {
     ROSE_ASSERT(this != NULL);

#if 0
     typedef std::pair<rose_graph_string_integer_hash_multimap::const_iterator,rose_graph_string_integer_hash_multimap::const_iterator> equal_range_type;
     equal_range_type equal_range_pair = p_string_to_node_index_multimap.equal_range(node->get_name());
     rose_graph_string_integer_hash_multimap::const_iterator lower_bound = equal_range_pair.first;
     rose_graph_string_integer_hash_multimap::const_iterator upper_bound = equal_range_pair.second;
     ROSE_ASSERT(lower_bound != upper_bound);
#endif

     std::set<int>  returnSet;
     
#ifdef ROSE_USE_NEW_GRAPH_NODES
     typedef std::pair<rose_graph_string_integer_hash_multimap::const_iterator,rose_graph_string_integer_hash_multimap::const_iterator> equal_range_type;
     equal_range_type equal_range_pair = p_string_to_node_index_multimap.equal_range(label);
     rose_graph_string_integer_hash_multimap::const_iterator lower_bound = equal_range_pair.first;
     rose_graph_string_integer_hash_multimap::const_iterator upper_bound = equal_range_pair.second;
     ROSE_ASSERT(lower_bound != upper_bound);

     for (rose_graph_string_integer_hash_multimap::const_iterator i = equal_range_pair.first; i != equal_range_pair.second; i++)
        {
          int node_index = i->second;
          printf ("Found node %d associated with label = %s \n",node_index,label.c_str());
          returnSet.insert(node_index);
        }
#endif

     return returnSet;
   }


bool
SgGraph::exists( SgGraphNode* node )
   {
     ROSE_ASSERT(this != NULL);
     ROSE_ASSERT(node != NULL);

#ifdef ROSE_USE_NEW_GRAPH_NODES
     return (p_node_index_to_node_map.find(node->get_index()) != p_node_index_to_node_map.end());
#else
     return false;
#endif
   }

bool
SgGraph::exists( SgGraphEdge* edge )
   {
     ROSE_ASSERT(this != NULL);
     ROSE_ASSERT(edge != NULL);

#ifdef ROSE_USE_NEW_GRAPH_NODES
     return (p_edge_index_to_edge_map.find(edge->get_index()) != p_edge_index_to_edge_map.end());
#else
     return false;
#endif
   }




// DQ (4/28/2009): Added support to build node (automaticly added to SgGraph).
SgGraphNode*
SgGraph::addNode( const std::string & name, SgNode* internal_node )
   {
     ROSE_ASSERT(this != NULL);

     SgGraphNode* node = new SgGraphNode(name);
     ROSE_ASSERT(node != NULL);

  // Set the reference to the associated AST node
     node->set_SgNode(internal_node);

     return addNode(node);
   }

SgGraphNode*
SgGraph::addNode( SgGraphNode* node )
   {
     ROSE_ASSERT(this != NULL);
     ROSE_ASSERT(node != NULL);

#ifdef ROSE_USE_NEW_GRAPH_NODES

     node->set_parent(this);

#ifdef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
  // Set this to aunique value for the associated SgGraph object.
     node->set_graph_id(get_index());

     typedef SgGraphNodeList::iterator node_iterator;
     node_iterator name_iterator = get_nodes()->get_nodes().find(node->get_name());
     bool matchingNameIsNew = name_iterator == get_nodes()->get_nodes().end();

     if (matchingNameIsNew == false)
        {
          matchingNameIsNew = (name_iterator->second->get_index() != node->get_index());
        }

     if (matchingNameIsNew == true)
        {
       // Build a new entry in the map!
       // printf ("Adding unique node to graph = %s for node = %p = %s (name = %s) \n",p_name.c_str(),node,node->class_name().c_str(),name.c_str());

       // Need the more uniform syntax when using hash_map
       // graph[name] = node;
          get_nodes()->get_nodes().insert(pair<string,SgGraphNode*>(node->get_name(),node));
        }
       else
        {
       // This name already exists in the graph.
          printf ("Error: This node = %p has a name = %s that already exists in the graph -- node = %p = %s \n",node,node->get_name().c_str(),node,node->class_name().c_str());
          ROSE_ASSERT(false);
        }
#else
     if (exists(node) == false)
        {
          int node_index = node->get_index();
          p_node_index_to_node_map[node_index] = node;

       // This fails for test_graph_5.C
       // ROSE_ASSERT(p_string_to_node_index_multimap.find(node->get_name()) == p_string_to_node_index_multimap.end());

       // Adding the lable to the label map (we might decide to not put empty labels into the map).
       // p_string_to_node_index_multimap[node->get_name()] = node_index;
          p_string_to_node_index_multimap.insert(std::pair<std::string,int>(node->get_name(),node_index));
        }
       else
        {
          printf ("This node already exists in this graph \n");
          ROSE_ASSERT(false);
        }
#endif
#endif

     return node;
   }





SgGraphEdge*
SgGraph::addEdge( SgGraphNode* a, SgGraphNode* b, const std::string & name )
   {
     ROSE_ASSERT(this != NULL);

     SgGraphEdge* edge = NULL;

#ifdef ROSE_USE_NEW_GRAPH_NODES
     edge = new SgGraphEdge(a,b,name);
     ROSE_ASSERT(edge != NULL);
#endif
     return addEdge(edge);
   }

// DQ (4/28/2009): Added support to build node (automaticly added to SgGraph).
SgGraphEdge*
SgGraph::addEdge( SgGraphEdge* edge )
   {
     ROSE_ASSERT(this != NULL);

     edge->set_parent(this);

#ifdef ROSE_USE_NEW_GRAPH_NODES
#ifdef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
  // Set this to aunique value for the associated SgGraph object.
     edge->set_graph_id(get_index());

     typedef SgGraphEdgeList::iterator edge_iterator;

     edge_iterator edgeIt = get_edges()->get_edges().find(edge->get_node_A());
  // bool matchingEdgeIsNew = ( (edgeIt == get_edges()->get_edges().end()) && ( edgeIt->second->get_node_B() != b) );
     bool matchingEdgeIsNew = (edgeIt == get_edges()->get_edges().end());
     if (matchingEdgeIsNew == false)
        {
       // printf ("The node has a valid edge, now check the edge's other end \n");
          matchingEdgeIsNew = ( edgeIt->second->get_node_B() != edge->get_node_B());
        }

  // printf ("matchingEdgeIsNew = %s \n",matchingEdgeIsNew ? "true" : "false");
     if (matchingEdgeIsNew == true)
        {
       // Build a new entry in the map!
       // printf ("Adding unique edge to graph = %s for edge = %p = %s (name = %s) \n",p_name.c_str(),edge,edge->class_name().c_str(),name.c_str());

       // Need the more uniform syntax when using hash_map
       // graph[name] = edge;
          get_edges()->get_edges().insert(pair<SgGraphNode*,SgGraphEdge*>(edge->get_node_A(),edge));
        }
       else
        {
       // This is something that we can test.
          ROSE_ASSERT(edgeIt->first == edge->get_node_A() || edgeIt->first == edge->get_node_B() );

       // This name already exists in the graph.
#if 1
          printf ("Warning: This edge = %p has a name = %s that already exists in the graph -- edge = %p = %s \n",edge,edge->get_name().c_str(),edge,edge->class_name().c_str());
#else
          printf ("Error: This edge = %p has a name = %s that already exists in the graph -- edge = %p = %s \n",edge,edge->get_name().c_str(),edge,edge->class_name().c_str());
          ROSE_ASSERT(false);
#endif
        }
#else
     if (exists(edge) == false)
        {
          int edge_index = edge->get_index();
          p_edge_index_to_edge_map[edge_index] = edge;

          ROSE_ASSERT(edge->get_node_A() != NULL);
          ROSE_ASSERT(edge->get_node_B() != NULL);

          int node_index_first  = edge->get_node_A()->get_index();
          int node_index_second = edge->get_node_B()->get_index();

       // Note that this significantly slows down the performance of the new graph support (appears to be about a factor of 10X).
       // Is there a better (faster) way to build the p_node_index_pair_to_edge_multimap?

       // DQ (5/2/2009): Note that operator[] is not available for the multimap container.
       // p_node_index_pair_to_edge_multimap[std::pair<int,int>(edge->get_node_A(),edge->get_node_B())] = edge;
#if 1
       // p_node_index_pair_to_edge_multimap.insert(std::pair<std::pair<int,int>,SgGraphEdge*>(std::pair<int,int>(edge->get_node_A()->get_index(),edge->get_node_B()->get_index()),edge));
          p_node_index_pair_to_edge_multimap.insert(std::pair<std::pair<int,int>,SgGraphEdge*>(std::pair<int,int>(node_index_first,node_index_second),edge));
#endif

       // Initialize the node index --> SgGraphEdge* multimap.
          printf ("In SgGraph::addEdge(): Insert edge (%d,%d) on node %d \n",node_index_first,node_index_second,node_index_first);
          p_node_index_to_edge_multimap.insert(std::pair<int,SgGraphEdge*>(node_index_first,edge));
        }
       else
        {
          printf ("This node already exists in this graph \n");
          ROSE_ASSERT(false);
        }
#endif
#endif

     return edge;
   }







#ifdef ROSE_USE_NEW_GRAPH_NODES
// Required for Boost Spanning Tree support.
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#endif

// std::vector <SgGraph::BoostEdgeDescriptor>
std::vector <SgGraphEdge*>
SgGraph::generateSpanningTree()
   {
     ROSE_ASSERT(this != NULL);

  // return rose_spanning_tree;
     std::vector <SgGraphEdge*> returnEdgeList;

#ifdef ROSE_USE_NEW_GRAPH_NODES

#ifdef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
     ROSE_ASSERT(get_edges() != NULL);

  // Assemble the representation of the edges for Boost.
     SgGraphEdgeList::iterator edgeIt = get_edges()->get_edges().begin();
     while(edgeIt != get_edges()->get_edges().end())
        {
       // We want the edges to have an index ordering (could be computed or saved, for now we will save it explicitly).
          int i = edgeIt->second->get_node_A()->get_index();
          int j = edgeIt->second->get_node_B()->get_index();

       // Generate the edge vector for MST (required for many BGL algorithms)
          p_boost_edges.push_back(BoostEdgeType(i,j));

       // Use constant edge weight for now (I think that the edge weights are required by the MST algorithm)
          p_boost_edge_weights.push_back(1);

          edgeIt++;
        }

     ROSE_ASSERT(get_nodes() != NULL);

  // DQ (4/29/2009): Boost Graph Type used to represent ROSE graphs within Boost Graph Algorithms.
     typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property < boost::edge_weight_t, int > > BoostGraphType;

  // Supporting graph type required by Boost Graph Library.
     typedef boost::graph_traits < BoostGraphType >::edge_descriptor   BoostEdgeDescriptor;

     BoostGraphType rose_BGL_graph(p_boost_edges.begin(), p_boost_edges.end(), p_boost_edge_weights.begin(), get_nodes()->get_nodes().size());

  // If we want to add edge weights then this is now we have to do it.
  // property_map < Graph, edge_weight_t >::type weight = get(edge_weight, g);

  // Result
     std::vector < BoostEdgeDescriptor > rose_spanning_tree;

#if 1
     printf ("Computing the kruskal_minimum_spanning_tree() \n");
     boost::kruskal_minimum_spanning_tree(rose_BGL_graph, std::back_inserter(rose_spanning_tree));
     printf ("DONE: Computing the kruskal_minimum_spanning_tree() \n");

     printf ("rose_spanning_tree.size() = %zu \n",rose_spanning_tree.size());
#endif

#if 0
     std::cout << "Print the edges in the MST:" << std::endl;
     for (std::vector < Edge >::iterator ei = spanning_tree.begin(); ei != spanning_tree.end(); ++ei)
        {
          cout << source(*ei, g) << " <--> " << target(*ei, g)
               << " with weight of " << weight[*ei]
               << endl;

          int s = source(*ei, g);
          int t = target(*ei, g);
          SgGraphNode* nodeA = get_nodes()->get_nodes()[s];
          SgGraphNode* nodeB = get_nodes()->get_nodes()[t];

       // Now to we find the edge?
       // SgGraphEdge* edge = nodeA->get_edges()[s];

       // How to I get the edge that ends at t?

          returnEdgeList.push_back(*ei);
        }
#endif

// endif for ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
#endif

// endif for ROSE_USE_NEW_GRAPH_NODES
#endif

     return returnEdgeList;
   }

