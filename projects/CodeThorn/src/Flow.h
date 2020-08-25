#ifndef FLOW_H
#define FLOW_H

#define USE_SAWYER_GRAPH

#include "SgNodeHelper.h"
#include "Labeler.h"

#include "Sawyer/Graph.h"

namespace CodeThorn {

  enum EdgeType { EDGE_UNKNOWN=0, EDGE_FORWARD, EDGE_BACKWARD, EDGE_TRUE, EDGE_FALSE, EDGE_LOCAL, EDGE_CALL, EDGE_CALLRETURN, EDGE_EXTERNAL, EDGE_PATH };

  class Edge;
  typedef std::set<Edge> EdgeSet;
  typedef std::set<EdgeType> EdgeTypeSet;
  
#ifdef USE_SAWYER_GRAPH
  struct EdgeData {
    EdgeData(EdgeTypeSet t, std::string a) : edgeTypes(t), annotation(a) {} 
    EdgeTypeSet edgeTypes;
    std::string annotation;
  };

  typedef Sawyer::Container::Graph<Label, EdgeData, Label> SawyerCfg;  
#endif

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
    // is true if specified type is present in the edge annotation, or it is of type EDGE_PATH.
    bool isType(EdgeType et) const;
    // If type EDGE_PATH is added then only EDGE_PATH is represented (all others are removed).
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

    /* an edge is valid if source and target labels are valid
       a default constructed edge is not valid */
    bool isValid() const;

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

  class Flow {
  public:  
#ifdef USE_SAWYER_GRAPH
    class iterator : public SawyerCfg::EdgeIterator {
    public: 
      iterator(const SawyerCfg::EdgeIterator& it) : SawyerCfg::EdgeIterator(it) {}
      Edge operator*();
      iterator& operator++() { SawyerCfg::EdgeIterator::operator++(); return *this; }
      iterator operator++(int) { return iterator(SawyerCfg::EdgeIterator::operator++(1)); }
      EdgeTypeSet getTypes();
      void setTypes(EdgeTypeSet types);
      std::string getAnnotation();
      Label source();
      Label target();
    private:
      Edge* operator->();
    };

    // schroder3 (2016-08-11): Replaced node iterator wrapper classes by typedefs of the
    //  already existing Sawyer graph vertex value iterator classes.
    // schroder3 (2016-08-08): Added node iterators:
    typedef SawyerCfg::VertexValueIterator node_iterator;
    typedef SawyerCfg::ConstVertexValueIterator const_node_iterator;
#else
    typedef std::set<Edge>::iterator iterator;
#endif
    Flow();

    // Edge iterators:
    iterator begin();
    iterator end();

    // schroder3 (2016-08-08): Added node iterators:
    node_iterator nodes_begin();
    node_iterator nodes_end();
    const_node_iterator nodes_begin() const;
    const_node_iterator nodes_end() const;
    const_node_iterator nodes_cbegin() const;
    const_node_iterator nodes_cend() const;

    Flow operator+(Flow& s2);
    Flow& operator+=(Flow& s2);
    std::pair<Flow::iterator, bool> insert(Edge e);
    Flow::iterator find(Edge e);
    bool contains(Edge e);
    bool contains(Label l);
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
#ifdef USE_SAWYER_GRAPH
    boost::iterator_range<Flow::iterator> inEdgesIterator(Label label);
    boost::iterator_range<Flow::iterator> outEdgesIterator(Label label);
    // schroder3 (2016-08-16): Returns a topological sorted list of CFG-edges
    std::list<Edge> getTopologicalSortedEdgeList(Label startLabel);
#endif
    Flow edgesOfType(EdgeType edgeType);
    Flow outEdgesOfType(Label label, EdgeType edgeType);
    // this function only returns a valid edge if exactly one edge exists
    Edge outEdgeOfType(Label label, EdgeType edgeType);

    Label getStartLabel() { return _startLabel; }
    void setStartLabel(Label label) { _startLabel = label; }
    void setDotOptionDisplayLabel(bool opt);
    void setDotOptionDisplayStmt(bool opt);
    void setDotOptionEdgeAnnotationsOnly(bool opt);
    void setDotOptionFixedColor(bool opt);
    void setDotFixedColor(std::string color);
    void setDotFixedNodeColor(std::string color);
    void setDotOptionHeaderFooter(bool opt);
    std::string toDot(Labeler *labeler);
    void setTextOptionPrintType(bool opt);
    void resetDotOptions();
    std::string toString();
    // deletes all Edges of type edgeType. The return value is the number of deleted edges.
    std::size_t deleteEdges(EdgeType edgeType);
    std::size_t deleteEdges(Flow& flow);
    void establishBoostGraph();

    //! inverts all edges in the graph. The root node is updated. This operation is only successful if
    //! the original graph had exactly one final node (which becomes the start node of the new graph).
    CodeThorn::Flow reverseFlow();
  private:
    bool _dotOptionDisplayLabel;
    bool _dotOptionDisplayStmt;
    bool _dotOptionEdgeAnnotationsOnly;
    bool _stringNoType;
    bool _dotOptionFixedColor;
    std::string _fixedColor;
    std::string _fixedNodeColor;
    bool _dotOptionHeaderFooter;
    Label _startLabel;
#ifdef USE_SAWYER_GRAPH
    SawyerCfg  _sawyerFlowGraph;
#else
    std::set<Edge> _edgeSet;
#endif
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
