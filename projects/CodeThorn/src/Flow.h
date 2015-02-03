#ifndef FLOW_H
#define FLOW_H

#include <boost/graph/adjacency_list.hpp> 
#include <boost/graph/graphviz.hpp>
using namespace std;
using namespace boost; 
using namespace SPRAY;

namespace CodeThorn {

#include "SgNodeHelper.h"
#include "Labeler.h"
#include "CommandLineOptions.h"

  //#include <boost/graph/graphviz.hpp>

  enum EdgeType { EDGE_UNKNOWN=0, EDGE_FORWARD, EDGE_BACKWARD, EDGE_TRUE, EDGE_FALSE, EDGE_LOCAL, EDGE_CALL, EDGE_CALLRETURN, EDGE_EXTERNAL, EDGE_PATH };

  class Edge;
  typedef set<Edge> EdgeSet;

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class Edge {
 public:
  Edge();
  Edge(Label source0,Label target0);
  Edge(Label source0,EdgeType type0,Label target0);
  Edge(Label source0,set<EdgeType> type0,Label target0);
  string toString() const;
  string toStringNoType() const;
  string toDotColored() const;
  string toDotFixedColor(string) const;
  //string typeToString() const;
  string typesToString() const;
  static string typeToString(EdgeType et);
  Label source;
  //EdgeType type;
  Label target;
  bool isType(EdgeType et) const;
  void addType(EdgeType et);
  void addTypes(set<EdgeType> ets);
  void removeType(EdgeType et);
  set<EdgeType> types() const;
  long typesCode() const;
  string color() const;
  string dotEdgeStyle() const;
  long hash() const;
  set<EdgeType> getTypes() const { return _types; }
 private:
  set<EdgeType> _types;
};

 bool operator==(const Edge& e1, const Edge& e2);
 bool operator!=(const Edge& e1, const Edge& e2);
 bool operator<(const Edge& e1, const Edge& e2);

/*! 
 * \author Markus Schordan
  * \date 2012.
 */
 class Flow : public set<Edge> {
 public:  
  Flow();
  Flow operator+(Flow& s2);
  Flow& operator+=(Flow& s2);
  LabelSet nodeLabels();
  LabelSet sourceLabels();
  LabelSet targetLabels();
  LabelSet pred(Label label);
  LabelSet succ(Label label);
  LabelSet reachableNodes(Label start);

  //LabelSet reachableNodesOnPath(Label start, Label target);
  /* computes all nodes which are reachable in the graph from the start node. A path is terminated by either the target node 
    or by a node without successors. The start node is not included in the result set, except
     it is reachable on some path starting from the start node.
  */
  LabelSet reachableNodesButNotBeyondTargetNode(Label start, Label target);

  Flow inEdges(Label label);
  Flow outEdges(Label label);
  Flow edgesOfType(EdgeType edgeType);
  Flow outEdgesOfType(Label label, EdgeType edgeType);
  void setDotOptionDisplayLabel(bool opt);
  void setDotOptionDisplayStmt(bool opt);
  void setDotOptionFixedColor(bool opt);
  void setDotFixedColor(string color);
  void setDotOptionHeaderFooter(bool opt);
  string toDot(Labeler *labeler);
  void setTextOptionPrintType(bool opt);
  void resetDotOptions();
  string toString();
  // deletes all Edges of type edgeType. The return value is the number of deleted edges.
  size_t deleteEdges(EdgeType edgeType);
  size_t deleteEdges(Flow& flow);
  void establishBoostGraph();

  //Define the graph using those classes
  typedef adjacency_list<listS, vecS, directedS, Label, set<EdgeType> > FlowGraph;
  //Some typedefs for simplicity
  typedef graph_traits<FlowGraph>::vertex_descriptor vertex_t;
  typedef graph_traits<FlowGraph>::edge_descriptor edge_t;
  void boostify();
  //! inverts all edges in the graph. The root node is updated. This operation is only successful if
  //! the original graph had exactly one final node (which becomes the start node of the new graph).
  Flow reverseFlow();

 private:
  bool _dotOptionDisplayLabel;
  bool _dotOptionDisplayStmt;
  bool _stringNoType;
  bool _dotOptionFixedColor;
  string _fixedColor;
  bool _dotOptionHeaderFooter;
  bool _boostified;

  FlowGraph _flowGraph;
};

class InterEdge {
 public:
  InterEdge(Label call, Label entry, Label exit, Label callReturn);
  string toString() const;
  Label call;
  Label entry;
  Label exit;
  Label callReturn;
};

bool operator<(const InterEdge& e1, const InterEdge& e2);
bool operator==(const InterEdge& e1, const InterEdge& e2);
bool operator!=(const InterEdge& e1, const InterEdge& e2);

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class InterFlow : public set<InterEdge> {
 public:
  string toString() const;
};

} // end namespace CodeThorn

#endif
