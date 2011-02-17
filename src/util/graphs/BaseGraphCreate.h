
#ifndef BASE_GRAPH_CREATE
#define BASE_GRAPH_CREATE

#include <MultiGraphCreate.h>
#include <GraphAccess.h>
#include <assert.h>

//Both nodes and edges are multigraph elements (could be shared by mulitiple graphs) 
typedef MultiGraphElemTemplate<void*> BaseGraphNode;
typedef MultiGraphElemTemplate<void*> BaseGraphEdge;

//Base interface of graph creation: create/delete nodes and edges
// with additional interfaces to multigraph and graph access
class BaseGraphCreate 
  : public MultiGraphCreate,
#if _MSC_VER
          public
#endif
           GraphAccessTemplate<BaseGraphNode,BaseGraphEdge>
{
 protected:
  typedef GraphAccessTemplate<BaseGraphNode,BaseGraphEdge> GraphAccessBase;
 public:
  //Arbitrary node and edge information
  typedef void* NodeContent;
  typedef void* EdgeContent;
  typedef GraphAccessBase::Node Node;
  typedef GraphAccessBase::Edge Edge;
  typedef GraphAccessBase::NodeIterator NodeIterator;
  typedef GraphAccessBase::EdgeIterator EdgeIterator;
 
 
  virtual ~BaseGraphCreate() {}

  virtual BaseGraphNode* CreateNode(NodeContent _id) =0;
  virtual BaseGraphEdge* CreateEdge(BaseGraphNode* src, BaseGraphNode *snk, EdgeContent _id)=0;
  virtual void MoveEdgeEndPoint( BaseGraphEdge *e, BaseGraphNode *n, EdgeDirection dir)=0;
  virtual void DeleteNode( BaseGraphNode *n)=0;
  virtual void DeleteEdge( BaseGraphEdge *n)=0;

  GraphAccessBase::GetNodeIterator;
  GraphAccessBase::GetNodeEdgeIterator;
  GraphAccessBase::GetEdgeEndPoint;
  GraphAccessBase::ContainNode;
  GraphAccessBase::ContainEdge;
};

// A wrapper on top of a graph implementation to implement base graph create interface
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
// DQ (8/31/2009): This is required for EDG to compile this code!
#ifdef USE_ROSE
                   ::GraphAccess::EdgeDirection dir)
#else  
                     GraphAccess::EdgeDirection dir)
#endif  
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
// DQ (8/31/2009): This is required for EDG to compile this code!
#ifdef USE_ROSE
                               ::GraphAccess::EdgeDirection dir) const
#else  
                                 GraphAccess::EdgeDirection dir) const
#endif                                 
   { return new IteratorImplTemplate<Edge*,typename GraphImpl::EdgeIterator>
             (impl->GetNodeEdgeIterator(static_cast<const typename GraphImpl::Node*>(n),
                                        TranslateDirection(dir))); }
  Node* GetEdgeEndPoint( const BaseGraphEdge* e, 
// DQ (8/31/2009): This is required for EDG to compile this code!
#ifdef USE_ROSE  
                            ::GraphAccess::EdgeDirection dir) const
#else
                              GraphAccess::EdgeDirection dir) const
#endif                                
    { return  impl->GetEdgeEndPoint(static_cast<const typename GraphImpl::Edge*>(e), 
                                    TranslateDirection(dir)); }
  bool ContainNode( const BaseGraphNode* n) const 
      { return impl->ContainNode(static_cast<const typename GraphImpl::Node*>(n)); }
  bool ContainEdge( const BaseGraphEdge* n) const 
      { return impl->ContainEdge(static_cast<const typename GraphImpl::Edge*>(n)); }
 protected:
  GraphImpl* impl;

  typename GraphImpl::EdgeDirection
// DQ (8/31/2009): This is required for EDG to compile this code!
#ifdef USE_ROSE  
  TranslateDirection( ::GraphAccess::EdgeDirection dir) const
#else  
  TranslateDirection( GraphAccess::EdgeDirection dir) const
#endif  
   {
     switch (dir)
        {
  // DQ (8/31/2009): This is required for EDG to compile this code!
#ifdef USE_ROSE 
          case ::GraphAccess::EdgeOut: 
#else
          case GraphAccess::EdgeOut: 
#endif
               return GraphImpl::EdgeOut;

       // DQ (8/31/2009): This is required for EDG to compile this code!
#ifdef USE_ROSE
          case ::GraphAccess::EdgeIn: 
#else
          case GraphAccess::EdgeIn: 
#endif
               return GraphImpl::EdgeIn;

          default:
               assert(false);
        }

  // DQ (11/28/2009): This should be unreachable code, but MSVC warns if a path does not contain a return stmt.
     assert(false);
     return GraphImpl::EdgeIn;
   }

};

#endif
