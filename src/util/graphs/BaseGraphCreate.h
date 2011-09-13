
#ifndef BASE_GRAPH_CREATE
#define BASE_GRAPH_CREATE

#include <MultiGraphCreate.h>
#include <GraphAccess.h>
#include <assert.h>

typedef MultiGraphElemTemplate<void*> BaseGraphNode;
typedef MultiGraphElemTemplate<void*> BaseGraphEdge;

class BaseGraphCreate 
  : public MultiGraphCreate,
          public
           GraphAccessTemplate<BaseGraphNode,BaseGraphEdge>
{
 protected:
  typedef GraphAccessTemplate<BaseGraphNode,BaseGraphEdge>
      GraphAccessBase;
 public:
  typedef void* NodeContent;
  typedef void* EdgeContent;
  typedef GraphAccessBase::Node Node;
  typedef GraphAccessBase::Edge Edge;
  typedef GraphAccessBase::NodeIterator NodeIterator;
  typedef GraphAccessBase::EdgeIterator EdgeIterator;
 
 
  virtual ~BaseGraphCreate() {}

  virtual BaseGraphNode* CreateNode(NodeContent _id) =0;
  virtual BaseGraphEdge* 
  CreateEdge(BaseGraphNode* src, BaseGraphNode *snk, EdgeContent _id)=0;
  virtual void 
  MoveEdgeEndPoint( BaseGraphEdge *e, BaseGraphNode *n, EdgeDirection dir)=0;
  virtual void DeleteNode( BaseGraphNode *n)=0;
  virtual void DeleteEdge( BaseGraphEdge *n)=0;

  GraphAccessBase::GetNodeIterator;
  GraphAccessBase::GetNodeEdgeIterator;
  GraphAccessBase::GetEdgeEndPoint;
  GraphAccessBase::ContainNode;
  GraphAccessBase::ContainEdge;
};

template <class GraphImpl>
class BaseGraphCreateWrap : public BaseGraphCreate
{
 public:
  BaseGraphCreateWrap() {  impl = new GraphImpl(); }
  virtual ~BaseGraphCreateWrap() { delete impl; }

  virtual BaseGraphNode* CreateNode(NodeContent _id) 
      { return new typename GraphImpl::Node( impl, this, _id); }
  virtual BaseGraphEdge* 
  CreateEdge(BaseGraphNode* src, BaseGraphNode *snk, EdgeContent _id)
   { 
      assert( ContainNode(src) && ContainNode(snk));
      return new typename GraphImpl::Edge( this, 
                          static_cast<typename GraphImpl::Node*>(src), 
                          static_cast<typename GraphImpl::Node*>(snk), _id); 
   }
  virtual void 
  MoveEdgeEndPoint( BaseGraphEdge *e, BaseGraphNode *n, 
                     GraphAccess::EdgeDirection dir)
    { 
      assert( ContainEdge(e) && ContainNode(n));
      static_cast<typename GraphImpl::Edge*>(e)->MoveEndPoint( static_cast<typename GraphImpl::Node*>(n), 
                                               TranslateDirection(dir));
    }
  virtual void DeleteNode( BaseGraphNode *n)
    { delete static_cast<typename GraphImpl::Node*>(n); }
  virtual void DeleteEdge( BaseGraphEdge *n)
    { delete static_cast<typename GraphImpl::Edge*>(n); }

  NodeIterator GetNodeIterator() const
    { return new IteratorImplTemplate<Node*,typename GraphImpl::NodeIterator>
            (impl->GetNodeIterator()); }
  EdgeIterator GetNodeEdgeIterator( const Node* n, 
                                 GraphAccess::EdgeDirection dir) const
   { return new IteratorImplTemplate<Edge*,typename GraphImpl::EdgeIterator>
             (impl->GetNodeEdgeIterator(static_cast<const typename GraphImpl::Node*>(n),
                                        TranslateDirection(dir))); }
  Node* GetEdgeEndPoint( const BaseGraphEdge* e, 
                                GraphAccess::EdgeDirection dir) const
    { return  impl->GetEdgeEndPoint(static_cast<const typename GraphImpl::Edge*>(e), 
                                    TranslateDirection(dir)); }
  bool ContainNode( const BaseGraphNode* n) const 
      { return impl->ContainNode(static_cast<const typename GraphImpl::Node*>(n)); }
  bool ContainEdge( const BaseGraphEdge* n) const 
      { return impl->ContainEdge(static_cast<const typename GraphImpl::Edge*>(n)); }
 protected:
  GraphImpl* impl;

  typename GraphImpl::EdgeDirection
  TranslateDirection( GraphAccess::EdgeDirection dir) const
  { switch (dir) {
   case GraphAccess::EdgeOut: return GraphImpl::EdgeOut;
   case GraphAccess::EdgeIn: return GraphImpl::EdgeIn;
   default:
     assert(false);
   }
  }

};

#endif
