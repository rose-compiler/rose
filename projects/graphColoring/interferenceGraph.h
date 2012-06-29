// interface function to generate interference graph of a program
// we consider only intra-procedural case for now
// Liao 5/14/2012
#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <algorithm>

//All work is within a single namespace
namespace graphColoring
{
  // Return true based on a given p probability, 
  // where p>=0.0 (0% chance) and p <=1.0 (100% chance) 
  bool trueOnProbability (float p, unsigned int seed);

  // interference graph
  // We use both adjacency matrix and adjacency list for an interference graph.
  // The two representations of the graph have to be consistent. 
  // The adjacency list is generated from an adjacency matrix
  //Efficient storage: 
  //For dense graph: number of edges is close to n*(n-1)/2 (complete graph)
  //   Lower diagonal makes more sense: empty right side , instead of empty left side of rows
  //For sparse graph: we use a map instead of a lower diagonal matrix to further save storage space
  //   Matrix storage cost: O(N^2) , when N is the number of nodes
  //   Using a map: storage cost: O(|E|), when |E| is the number of edges. if |E| is significantly smaller than O(N^2)(a complete graph)
  class Adjacent_Matrix {
    private:
      size_t n_count; // keep track the number of nodes of the graph
      std::map < std::pair <size_t, size_t>, bool> matrix; // a map between a pair of nodes and its corresponding edge existence value, true or false
    public:
      Adjacent_Matrix(size_t node_count);
     //set the edge between node i and j to be val 
      void setEdge(size_t index_i, size_t index_j, bool val=true);
      //reset the edge to be false
      void resetEdge(size_t index_i, size_t index_j);
      // switch edge from true to false , or to true if it is false. 
      void flipEdge(size_t index_i, size_t index_j);

      // check if there is an edge between two nodes, index starts from 0
      bool hasEdge (size_t index_i, size_t index_j);
      void print();
      void toDotGraph(std::string file_name);
      // TODO void toDotGraph(string file_name);
  };

  class Node {
    public:
      size_t id;
      // adjacent list
      // undirected simple graph
      Node(size_t i):id(i) {}
      //vector <Node*> in_neighbors;  // optional in neighbors
      std::vector <Node*> neighbors;
      friend std::ostream & operator<< (std::ostream & o, const Node &);
  };

  //---------------------------------------------------------------------------------
  class Graph {
    private:
      std::vector<bool> visited;
    public:
      Adjacent_Matrix* adj_matrix;
      std::vector <Node*> nodes; // nodes with index to a vector
      // create a graph with a specified node count, and edge count equal to rate* N*(N-1)/2
      // complete rate of 1.0 means the graph is a complete graph: each node has an edge to each of other nodes
      void randomPopulateGraph(size_t node_count, float edge_complete_rate);
      // Construct a graph with node_count nodes, edge complete rate
      Graph (size_t node_count, float edge_complete_rate);
      size_t getNodeCount(){return nodes.size();}

      // Construct a graph from a span tree
      Graph(size_t node_count, const std::vector <std::pair <Node*, Node*> >& span_tree_edges);

      ~Graph ();
      // depth first travese for the entire tree
      void DFS();

      // Depth first traverse at a given node, stores visited edges
      void depth_first_traverse (Node*n, std::vector< std::pair < Node*, Node*> >& span_tree_edges );
      // return the neighbors of a node n
      void getNeighbors (const Node& n, std::vector<Node*>& neighbors);
      // Using the adjacency matrix to regenerate adjacent list (neighbors) for each node  
      void regenerateAdjacentNeighbors ();
      static void printSpanTree (const std::vector <std::pair <Node*, Node*> >& span_tree_edges);
      void initVisited(); // init visited flags
      void toDotGraph(std::string file_name)
      {
        adj_matrix-> toDotGraph(file_name);
      }
  };



} // end of namespace

