
#ifndef BASE_GRAPH_CREATE
#define BASE_GRAPH_CREATE

#include <GraphTemplate.h>

class GraphNodeToString
{public:
 static std::string ToString(const GraphNode* n) { return n->ToString(); }
};

class GraphEdgeToString
{ public:
   static std::string ToString(const GraphEdge* n) { return n->ToString(); }
};

class BaseGraphCreate : 
  public GraphAccessTemplate<GraphNodeTemplate<GraphNode*,GraphNode*, GraphNodeToString>, 
                            GraphEdgeTemplate<GraphEdge*,GraphEdge*, GraphEdgeToString> >, 
  public GraphCreate
{
//Boolean ContainNodeImpl( const GraphNode* n) const
  int ContainNodeImpl( const GraphNode* n) const
     { return GraphCreate::LinkedToElem(n); }
//Boolean ContainEdgeImpl( const GraphEdge* e) const
  int ContainEdgeImpl( const GraphEdge* e) const
     { return GraphCreate::LinkedToElem(e); }

 public:
  virtual ~BaseGraphCreate() {};
  virtual Node* CreateNode( GraphNode *id) = 0;
  virtual Edge* CreateEdge(Node *src, Node *snk, GraphEdge *id) = 0;

  virtual void MoveEdgeEndPoint( Edge *e, Node *n, 
                             EdgeDirection dir)=0;
  virtual void DeleteNode(Node *n) = 0;
  virtual void DeleteEdge(Edge *e) = 0;
};

template <class NodeImpl, class EdgeImpl, class GraphImpl>
class BaseGraphCreateTemplate : public BaseGraphCreate
{
 protected:
  GraphImpl* impl;

  typename GraphImpl::EdgeDirection
  TranslateDirection( GraphAccess::EdgeDirection dir) const
  { switch (dir) {
   case GraphAccess::EdgeOut: return GraphImpl::EdgeOut;
   case GraphAccess::EdgeIn: return GraphImpl::EdgeIn;

// DQ (12/31/2005): Added default to make clear that not all cases were handled
   default: 
     {
    // ignoring BiEdge case
     }
   }
   assert(false);
  }

  BaseGraphCreate::NodeIterator GetNodeIteratorImpl2() const
    { 
      return GetNodeIterator();
    }
  BaseGraphCreate::EdgeIterator 
  GetNodeEdgeIteratorImpl2(const Node *n, EdgeDirection dir) const
    { 
      return GetNodeEdgeIterator( n,  dir);
    }
  GraphNode* GetEdgeEndPointImpl(const GraphEdge *e, EdgeDirection dir) const
    { 
      return GetEdgeEndPoint( static_cast<const Edge*>(e), dir);
    }

 public:
  BaseGraphCreateTemplate() {  impl = new GraphImpl(); }
  virtual ~BaseGraphCreateTemplate() { delete impl; }

  Node* CreateNode(GraphNode *_id) { return new NodeImpl( impl, this, _id); }
  Edge* CreateEdge(Node* src, Node *snk, GraphEdge *_id)
   { 
      assert( ContainNode(src) && ContainNode(snk));
      return new EdgeImpl( this, static_cast<NodeImpl*>(src), static_cast<NodeImpl*>(snk), 
                       _id); 
   }
  void MoveEdgeEndPoint( Edge *e, Node *n, EdgeDirection dir)
    { 
      assert( ContainEdge(e) && ContainNode(n));
      static_cast<EdgeImpl*>(e)->MoveEndPoint( static_cast<NodeImpl*>(n), 
                                               TranslateDirection(dir));
    }
  void DeleteNode( Node *n)
    { delete static_cast<NodeImpl*>(n); }
  void DeleteEdge( Edge *n)
    { delete static_cast<EdgeImpl*>(n); }

  NodeIterator GetNodeIterator() const 
    {  return new Iterator2ImplTemplate 
                 <GraphNode*, Node*, typename GraphImpl::NodeIterator>
                ( impl->GetNodeIterator() ); 
    }
  EdgeIterator GetNodeEdgeIterator( const Node* n, EdgeDirection dir) const
    {  assert(ContainNode(n));
      return new Iterator2ImplTemplate 
                <GraphEdge*,Edge*, typename GraphImpl::EdgeIterator>
            ( impl->GetNodeEdgeIterator( static_cast<const NodeImpl*>(n), TranslateDirection(dir)));
    }
  NodeImpl* GetEdgeEndPoint( const Edge *e, EdgeDirection dir) const
    {  assert (ContainEdge(e));
      return  impl->GetEdgeEndPoint( static_cast<const EdgeImpl*>(e), TranslateDirection(dir));
    }
};

#endif
