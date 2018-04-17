#ifndef CONTROLFLOWGRAPH_H
#define CONTROLFLOWGRAPH_H

// #include "config.h"


#include "CFG.h"
#include <sstream>

// DQ (3/21/2006): Added namespace to separate the 
// PRE specific work (and it's use of the type named 
// ControlFlowGraph which conflicts with the OTHER
// ControlFlowGraph in:
//    src/midend/programAnalysis/dominatorTreesAndDominanceFrontiers
namespace PRE {

typedef int Vertex;
typedef int Edge;
typedef std::vector<std::vector<SgNode*> > NodeStatementsMap;
typedef std::vector<CFGConfig::EdgeType> EdgeTypeMap;
typedef std::vector<std::pair<SgNode*, bool> > EdgeInsertionPointMap;
typedef const std::vector<int>& Vertices;
typedef const std::vector<int>& Edges;
typedef std::vector<int>::const_iterator VertexIter;
typedef std::vector<int>::const_iterator OutEdgeIter;
typedef std::vector<int>::const_iterator InEdgeIter;
typedef std::vector<int>::const_iterator EdgeIter;

//! Graph structure for control flow graphs.  Based loosely on BGL graph type.
class simpleGraph {
  std::vector<int> all_vertices;
  std::vector<int> all_edges;
  std::vector<int> sources;
  std::vector<int> targets;
  std::vector<std::vector<int> > raw_out_edges;
  std::vector<std::vector<int> > raw_in_edges;

  public:
  int add_vertex() {
    all_vertices.push_back(all_vertices.size());
    raw_out_edges.push_back(std::vector<int>());
    raw_in_edges.push_back(std::vector<int>());
    return all_vertices.size() - 1;
  }

  int add_edge(int in, int out) {
    all_edges.push_back(all_edges.size());
    sources.push_back(in);
    targets.push_back(out);
    int edge_num = all_edges.size() - 1;
    raw_out_edges[in].push_back(edge_num);
    raw_in_edges[out].push_back(edge_num);
    return edge_num;
  }

  Vertices vertices() const {
    return all_vertices;
  }

  Edges edges() const {
    return all_edges;
  }

  Edges out_edges(int vertex) const {
    return raw_out_edges[vertex];
  }

  Edges in_edges(int vertex) const {
    return raw_in_edges[vertex];
  }

  int source(int edge) const {
    return sources[edge];
  }

  int target(int edge) const {
    return targets[edge];
  }
};

//! The whole CFG structure, including some external properties
struct myControlFlowGraph {
  simpleGraph graph;
  std::vector<std::vector<SgNode*> > node_statements;
  std::vector<CFGConfig::EdgeType> edge_type;
  std::vector<std::pair<SgNode*, bool> > edge_insertion_point;
};


// DQ (3/21/2006): We put this is it's own namespace because it conflicted with 
// the ControlFlowGraph built in dominatorTreesAndDominanceFrontiers/ControlFlowGraph.h
typedef myControlFlowGraph ControlFlowGraph;

//! Make the control flow graph for a given node
void makeCfg(SgFunctionDefinition* n, ControlFlowGraph& graph);

//! Add information to each edge on where to insert statements on that edge.
//! The insertion point structure is a pair of (SgNode*, bool), where the bool
//! is true for inserting before the given node and false for inserting after
//! it.
void addEdgeInsertionPoints(ControlFlowGraph& controlflow);

//! Print out a CFG as a dot file
void printCfgAsDot(std::ostream& dotfile,
                   const ControlFlowGraph& controlflow);

// closing scope of namespace: PRE
}

#endif // CONTROLFLOWGRAPH_H
