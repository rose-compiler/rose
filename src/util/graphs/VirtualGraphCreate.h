
#ifndef VIRTUAL_GRAPH_CREATE
#define VIRTUAL_GRAPH_CREATE

#include <PtrMap.h>
#include <BaseGraphCreate.h>
#include <GraphUtils.h>

template <class NodeImpl, class EdgeImpl>
class VirtualGraphCreateTemplate 
  : public MultiGraphCreate, public GraphAccessTemplate<NodeImpl,EdgeImpl>
{
 private:
  BaseGraphCreate *impl;
  PtrMapWrap <NodeImpl, BaseGraphNode> nodeMap;
  PtrMapWrap <EdgeImpl, BaseGraphEdge> edgeMap;

 public:
  typedef NodeImpl Node;
  typedef EdgeImpl Edge;
  typedef typename GraphAccess::EdgeDirection EdgeDirection;
  typedef typename GraphAccessTemplate<NodeImpl,EdgeImpl>::NodeIterator NodeIterator;
  typedef typename GraphAccessTemplate<NodeImpl,EdgeImpl>::EdgeIterator EdgeIterator;

  class EdgeIteratorImpl : public IteratorImpl<EdgeImpl*>
   { 
     const VirtualGraphCreateTemplate<NodeImpl,EdgeImpl> *gc;
     BaseGraphCreate::EdgeIterator baseIter;
     typedef GraphAccess::EdgeDirection EdgeDirection;
     EdgeIteratorImpl(const VirtualGraphCreateTemplate<NodeImpl,EdgeImpl> *_gc, 
                   const BaseGraphCreate::EdgeIterator &base)
          : gc(_gc), baseIter(base) {}
    public:
     EdgeIteratorImpl( const EdgeIteratorImpl &that)
          : gc(that.gc), baseIter(that.baseIter) {}
     EdgeIteratorImpl& operator = ( const EdgeIteratorImpl &that)
          { gc = that.gc; baseIter = that.baseIter; return *this; }

     void Reset() { baseIter.Reset(); }
     void Advance() { baseIter.Advance(); }
     void operator ++() { Advance(); }
     void operator ++(int) {Advance(); }
     Edge* Current() const { return gc->GetVirtualEdge( baseIter.Current() ); }
     Edge* operator*() const { return Current(); }
     IteratorImpl<EdgeImpl*>* Clone() const { return new EdgeIteratorImpl(*this); }
     bool ReachEnd() const { return baseIter.ReachEnd(); }
    friend class VirtualGraphCreateTemplate<NodeImpl,EdgeImpl>;
   };
   class NodeIteratorImpl  : public IteratorImpl<NodeImpl*>
   {  
     const VirtualGraphCreateTemplate<NodeImpl,EdgeImpl> *gc;
     BaseGraphCreate::NodeIterator baseIter;
     NodeIteratorImpl( const VirtualGraphCreateTemplate<NodeImpl,EdgeImpl> *_gc, 
                   const BaseGraphCreate::NodeIterator& base)
         : gc(_gc), baseIter(base) {}
   public:
     NodeIteratorImpl( const NodeIteratorImpl &that)
        : gc(that.gc), baseIter(that.baseIter) {}
     NodeIteratorImpl& operator = ( const NodeIteratorImpl &that)
          { gc = that.gc; baseIter = that.baseIter; return *this; }
     void Reset() { baseIter.Reset(); }
     void Advance() { baseIter.Advance(); }
     void operator ++() { Advance(); }
     void operator ++(int) {Advance(); }
     Node* Current() const { return gc->GetVirtualNode( baseIter.Current() ); }
     Node* operator*() const { return Current(); }
     bool ReachEnd() const { return baseIter.ReachEnd(); }
     IteratorImpl<NodeImpl*>* Clone() const { return new NodeIteratorImpl(*this); }
    friend class VirtualGraphCreateTemplate<NodeImpl,EdgeImpl>;
   };

 public:
  virtual ~VirtualGraphCreateTemplate();
  VirtualGraphCreateTemplate(BaseGraphCreate *_impl = 0);
  BaseGraphCreate* GetBaseGraph() { return impl; }

  BaseGraphNode* AddNode( Node* idNode);
  BaseGraphEdge* AddEdge( Node* idSrc, Node* idSnk, Edge* idEdge);
  void AddNodeSet( typename GraphAccessTemplate<NodeImpl,EdgeImpl>::NodeIterator iter);
  void AddGraph( const GraphAccessTemplate<NodeImpl,EdgeImpl> *dg);
  bool DeleteNode(Node* n);
  bool DeleteEdge(Edge* e);
  void MoveEdgeEndPoint(Edge *e, EdgeDirection dir, Node *n);

  BaseGraphNode* GetBaseNode( const Node *id) const;
  BaseGraphEdge* GetBaseEdge( const Edge *id) const;
  Node *GetVirtualNode(const BaseGraphNode *base) const;
  Edge *GetVirtualEdge(const BaseGraphEdge *base) const;
  bool IsBaseNode( const BaseGraphNode* n) const
   { return impl->ContainNode(n); }
  bool IsBaseEdge( const BaseGraphEdge* e) const
   { return impl->ContainEdge(e); }

  bool ContainNode( const Node* n) const
   { return GetBaseNode(n) != 0; }
  bool ContainEdge( const Edge* e) const
   { return GetBaseEdge(e) != 0; }
  Node* GetEdgeEndPoint( const Edge *e, EdgeDirection dir) const;
  EdgeIterator GetNodeEdgeIterator( const Node *id, EdgeDirection dir) const
       { 
          return new EdgeIteratorImpl(this, 
                     impl->GetNodeEdgeIterator( GetBaseNode(id), dir)) ; 
       }
  NodeIterator GetNodeIterator() const
        { return new NodeIteratorImpl(this, impl->GetNodeIterator()); }
};

#define TEMPLATE_ONLY
#include <VirtualGraphCreate.C>
#undef TEMPLATE_ONLY
#endif

