#ifndef FLOW_H
#define FLOW_H

//#define USE_SAWYER_GRAPH

#include <boost/graph/adjacency_list.hpp> 
#include <boost/graph/graphviz.hpp>
using namespace boost; 

#include "SgNodeHelper.h"
#include "Labeler.h"

#include "Sawyer/Graph.h"

namespace SPRAY {

  enum EdgeType { EDGE_UNKNOWN=0, EDGE_FORWARD, EDGE_BACKWARD, EDGE_TRUE, EDGE_FALSE, EDGE_LOCAL, EDGE_CALL, EDGE_CALLRETURN, EDGE_EXTERNAL, EDGE_PATH };

  class Edge;
  typedef std::set<Edge> EdgeSet;
  typedef std::set<EdgeType> EdgeTypeSet;
  
  struct EdgeData {
    EdgeData(EdgeTypeSet t, std::string a) : edgeTypes(t), annotation(a) {} 
    EdgeTypeSet edgeTypes;
    std::string annotation;
  };

  typedef Sawyer::Container::Graph<Label, EdgeData, Label> SawyerCfg;  

  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  class Edge {
  public:
    Edge();
    Edge(Label source0,Label target0);
    Edge(Label source0,EdgeType type0,Label target0);
    Edge(Label source0,EdgeTypeSet type0,Label target0);
    std::string toString() const;
    std::string toStringNoType() const;
    std::string toDotColored() const;
    std::string toDotFixedColor(std::string) const;
    std::string toDotAnnotationOnly() const; 
    std::string typesToString() const;
    static std::string typeToString(EdgeType et);
    bool isType(EdgeType et) const;
    void addType(EdgeType et);
    void addTypes(std::set<EdgeType> ets);
    void removeType(EdgeType et);
    EdgeTypeSet types() const;
    long typesCode() const;
    std::string color() const;
    std::string dotEdgeStyle() const;
    long hash() const;
    Label source() const { return _source; }
    Label target() const { return _target; }
    EdgeTypeSet getTypes() const { return _types; }
    std::string getAnnotation() const { return _annotation; }
    void setAnnotation(std::string annotation) { _annotation = annotation;}
  private:
    Label _source;
    Label _target;
    EdgeTypeSet _types;
    std::string _annotation;
  };
  
  bool operator==(const Edge& e1, const Edge& e2);
  bool operator!=(const Edge& e1, const Edge& e2);
  bool operator<(const Edge& e1, const Edge& e2);
  
  /*! 
   * \author Markus Schordan
   * \date 2012.
   */

  class Flow
#ifndef USE_SAWYER_GRAPH
  : public std::set<Edge>
#endif
  {
  public:  
#ifndef USE_SAWYER_GRAPH
    typedef std::set<Edge>::iterator iterator;
#else
    class iterator : public SawyerCfg::EdgeIterator {
    public: 
    iterator(const SawyerCfg::EdgeIterator& it) : SawyerCfg::EdgeIterator(it) {}
      Edge operator*();
      EdgeTypeSet getTypes();
      std::string getAnnotation();
      Label source();
      Label target();
    private:
      Edge* operator->();
    };
#endif
    Flow();
    iterator begin();
    iterator end();
    Flow operator+(Flow& s2);
    Flow& operator+=(Flow& s2);
    std::pair<Flow::iterator, bool> insert(Edge e);
    void erase(Flow::iterator iter);
    size_t erase(Edge e);
    size_t size();
    LabelSet nodeLabels();
    LabelSet sourceLabels();
    LabelSet targetLabels();
    LabelSet pred(Label label);
    LabelSet succ(Label label);
    LabelSet reachableNodes(Label start);
    std::set<std::string> getAllAnnotations();

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
    Label getStartLabel() { return _startLabel; }
    void setStartLabel(Label label) { _startLabel = label; }
    void setDotOptionDisplayLabel(bool opt);
    void setDotOptionDisplayStmt(bool opt);
    void setDotOptionEdgeAnnotationsOnly(bool opt);
    void setDotOptionFixedColor(bool opt);
    void setDotFixedColor(std::string color);
    void setDotOptionHeaderFooter(bool opt);
    std::string toDot(Labeler *labeler);
    void setTextOptionPrintType(bool opt);
    void resetDotOptions();
    std::string toString();
    // deletes all Edges of type edgeType. The return value is the number of deleted edges.
    std::size_t deleteEdges(EdgeType edgeType);
    std::size_t deleteEdges(Flow& flow);
    void establishBoostGraph();
    
    //Define the graph using those classes
    typedef adjacency_list<listS, vecS, directedS, Label, std::set<EdgeType> > FlowGraph;
    //Some typedefs for simplicity
    typedef graph_traits<FlowGraph>::vertex_descriptor vertex_t;
    typedef graph_traits<FlowGraph>::edge_descriptor edge_t;
    void boostify();
    //! inverts all edges in the graph. The root node is updated. This operation is only successful if
    //! the original graph had exactly one final node (which becomes the start node of the new graph).
    SPRAY::Flow reverseFlow();
    
  private:
    bool _dotOptionDisplayLabel;
    bool _dotOptionDisplayStmt;
    bool _dotOptionEdgeAnnotationsOnly;
    bool _stringNoType;
    bool _dotOptionFixedColor;
    std::string _fixedColor;
    bool _dotOptionHeaderFooter;
    bool _boostified;
    SawyerCfg  _sawyerFlowGraph;
    FlowGraph _flowGraph;
    Label _startLabel;
    std::set<Edge> _edgeSet;
  };
  
  class InterEdge {
  public:
    InterEdge(Label call, Label entry, Label exit, Label callReturn);
    std::string toString() const;
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
  class InterFlow : public std::set<InterEdge> {
  public:
    std::string toString() const;
  };

} // end namespace CodeThorn

#endif
