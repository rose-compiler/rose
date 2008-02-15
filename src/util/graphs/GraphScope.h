
#ifndef GRAPH_SCOPE_H
#define GRAPH_SCOPE_H

#include <GraphTemplate.h>
#include <IteratorCompound.h>
#include <assert.h>

template <class NodeImpl, class EdgeImpl, class Graph>
class GraphScopeTemplate : public GraphAccessTemplate<NodeImpl,EdgeImpl>
{
 public:
   typedef GraphAccessTemplate<NodeImpl,EdgeImpl> Base;
   typedef GraphAccess::EdgeDirection EdgeDirection;
 private:
  Graph impl;
  typename Base::NodeIterator GetNodeIteratorImpl2() const
     { return new Iterator2ImplTemplate<GraphNode*, typename Graph::Node*, typename Graph::NodeIterator>
                    (impl.GetNodeIterator()); }
  typename Base::EdgeIterator 
      GetNodeEdgeIteratorImpl2(const typename Base::Node *n, EdgeDirection dir) const
     { return new Iterator2ImplTemplate<GraphEdge*, typename Graph::Edge*, typename Graph::EdgeIterator>
                    (impl.GetNodeEdgeIterator(n,dir)); }
  GraphNode* GetEdgeEndPointImpl( const GraphEdge *e, EdgeDirection dir) const
   { return impl.GetEdgeEndPoint(static_cast<const typename Base::Edge*>(e), dir); }
//Boolean ContainNodeImpl(const GraphNode *n) const
  int ContainNodeImpl(const GraphNode *n) const
     { return impl.ContainNode(static_cast<const typename Base::Node*>(n)); }
//Boolean ContainEdgeImpl(const GraphEdge *e) const
  int ContainEdgeImpl(const GraphEdge *e) const
     { return impl.ContainEdge(static_cast<const typename Base::Edge*>(e)); }
 public:
  GraphScopeTemplate( const Graph& _impl) : impl(_impl) {}
};

template <class Graph, class Select1, class Select2>
class GraphSelectEndSet 
{
  const Graph* impl;
  Select1 srcsel;
  Select2 snksel;
 public:
  GraphSelectEndSet(const Graph* _impl, Select1 sel1, Select2 sel2)
   : impl(_impl), srcsel(sel1), snksel(sel2) {}
//Boolean operator()(const typename Graph::Edge* e) const
  int operator()(const typename Graph::Edge* e) const
    {return srcsel(impl->GetEdgeEndPoint(e, GraphAccess::EdgeOut)) &&
            snksel(impl->GetEdgeEndPoint(e, GraphAccess::EdgeIn));
    }
//Boolean operator()(const typename Graph::Node* n) const
  int operator()(const typename Graph::Node* n) const
    {return srcsel(n) || snksel(n); }
};

template <class Graph, class Select1>
class GraphSelectEndSet1 
{
  const Graph* impl;
  Select1 sel;
 public:
  typedef typename Graph::NodeIterator NodeIterator;
  typedef typename Graph::EdgeIterator EdgeIterator;
  GraphSelectEndSet1(const Graph* _impl, Select1 sel1)
   : impl(_impl), sel(sel1) {}
//Boolean operator()(const typename Graph::Edge* e) const
  int operator()(const typename Graph::Edge* e) const
    {
      bool b1 = sel(impl->GetEdgeEndPoint(e, GraphAccess::EdgeOut));
      bool b2 = sel(impl->GetEdgeEndPoint(e, GraphAccess::EdgeIn)); 
      return (b1 && !b2) || (!b1 && b2);
    }
//Boolean operator()(const typename Graph::Node* n) const
  int operator()(const typename Graph::Node* n) const
    {return true; }
};

template <class Graph, class Select1>
class GraphSelectEndSet2 
{
  const Graph* impl;
  Select1 sel;
 public:
  GraphSelectEndSet2(const Graph* _impl, Select1 sel1)
   : impl(_impl), sel(sel1) {}
//Boolean operator()(const typename Graph::Edge* e) const
  int operator()(const typename Graph::Edge* e) const
    {return sel(impl->GetEdgeEndPoint(e, GraphAccess::EdgeOut)) &&
            sel(impl->GetEdgeEndPoint(e, GraphAccess::EdgeIn)); }
//Boolean operator()(const typename Graph::Node* n) const
  int operator()(const typename Graph::Node* n) const
    {return sel(n); }
};
template <class Graph, class Select>
class GraphSelectEdge 
{
  Select sel;
 public:
  GraphSelectEdge(Select s) : sel(s) {}
  
//Boolean operator()(const typename Graph::Edge* e) const
  int operator()(const typename Graph::Edge* e) const
    {return sel(e); }
//Boolean operator()(const typename Graph::Node* n) const
  int operator()(const typename Graph::Node* n) const
    {return true; }
};

template <class Graph, class SelectNode, class SelectEdge>
class GraphSelectCompound : public SelectNode, public SelectEdge
{
 public:
  GraphSelectCompound(const SelectNode& selnode, const SelectEdge& seledge)
     : SelectNode(selnode), SelectEdge(seledge) {}
//Boolean operator()(const typename Graph::Edge* e) const
  int operator()(const typename Graph::Edge* e) const
    {return SelectNode::operator()(e) && 
            SelectEdge::operator()(e);
    }
//Boolean operator()(const typename Graph::Node* n) const
  int operator()(const typename Graph::Node* n) const
    {return SelectNode::operator()(n); }
};

template <class Graph, class Select>
class GraphSelect
{
  const Graph* impl;
  Select sel;
 public:
  typedef typename Graph::EdgeDirection EdgeDirection;
  typedef typename Graph::Node Node;
  typedef typename Graph::Edge Edge;
  typedef typename Graph::NodeIterator NodeIterator;
  typedef typename Graph::EdgeIterator EdgeIterator;

  GraphSelect(const Graph* _impl, const Select& s)
       : impl(_impl), sel(s) {}
  ~GraphSelect() {}

  typename Graph::NodeIterator GetNodeIterator() const 
    { 
       typedef SelectIterator<Node*,Node*,NodeIterator,Select> ThisSelectIterator;
       return new Iterator2ImplTemplate<GraphNode*,typename Graph::Node*,
                          ThisSelectIterator>( ThisSelectIterator(impl->GetNodeIterator(),sel));
    }

  typename Graph::EdgeIterator 
   GetNodeEdgeIterator(const typename Graph::Node *n, EdgeDirection dir) const
   { 
     typedef SelectIterator<Edge*,Edge*, EdgeIterator, Select> ThisSelectIterator;
     return new Iterator2ImplTemplate<GraphEdge*,typename Graph::Edge*,ThisSelectIterator>
               (ThisSelectIterator(impl->GetNodeEdgeIterator(n,dir), sel) ); 
   }
  typename Graph::Node* GetEdgeEndPoint(const typename Graph::Edge* e, EdgeDirection dir) const
   { return impl->GetEdgeEndPoint(e, dir); }
//Boolean ContainEdge( const typename Graph::Edge* e) const
  int ContainEdge( const typename Graph::Edge* e) const
    {
      return sel(e);
    }
//Boolean ContainNode( const typename Graph::Node * n) const
  int ContainNode( const typename Graph::Node * n) const
    { return sel(n); }
};

template <class Graph>
class GraphReverseEdge 
{
  Graph* impl;
 public:
  typedef typename Graph::Node Node;
  typedef typename Graph::Edge Edge;
  typedef typename Graph::NodeIterator NodeIterator;
  typedef typename Graph::EdgeIterator EdgeIterator;
  GraphReverseEdge(Graph* _impl) : impl(_impl) {}
  ~GraphReverseEdge() {}
  typename Graph::EdgeIterator 
   GetNodeEdgeIterator(const typename Graph::Node *n, GraphAccess::EdgeDirection dir) const
   {return impl->GetNodeEdgeIterator(n, GraphAccess::Reverse(dir) ); }
  typename Graph::Node* 
    GetEdgeEndPoint( const typename Graph::Edge *e, GraphAccess::EdgeDirection dir) const
   { return impl->GetEdgeEndPoint(e, GraphAccess::Reverse(dir) ); }

  typename Graph::NodeIterator GetNodeIterator() const { return impl->GetNodeIterator(); }
//Boolean ContainEdge( const typename Graph::Edge* e) const {return impl->ContainEdge(e); }
  int ContainEdge( const typename Graph::Edge* e) const {return impl->ContainEdge(e); }
//Boolean ContainNode( const typename Graph::Node* n) const { return impl->ContainNode(n); }
  int ContainNode( const typename Graph::Node* n) const { return impl->ContainNode(n); }
};
typedef GraphScopeTemplate<GraphNode,GraphEdge,GraphReverseEdge<GraphAccess> > GraphReverseEdgeImpl;
         

#endif
