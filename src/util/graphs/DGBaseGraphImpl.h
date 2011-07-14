
#ifndef DG_BASE_GRAPH_IMPL
#define DG_BASE_GRAPH_IMPL

#include <DAG.h>
#include <assert.h>
#include <BaseGraphCreate.h>
#include <sstream>

// definition of DGBaseGraphImpl
class DGBaseEdgeImpl;
class DGBaseNodeImpl 
   : public DirectedGraphNode<DGBaseNodeImpl,DGBaseEdgeImpl>, 
     public BaseGraphNode
{
 public:
  DGBaseNodeImpl( DirectedGraph<DGBaseNodeImpl,DGBaseEdgeImpl> *g1, 
                  BaseGraphCreate *g2, BaseGraphCreate::NodeContent _id)
    : DirectedGraphNode<DGBaseNodeImpl,DGBaseEdgeImpl>(g1), 
      BaseGraphNode(g2,_id) {}
  ~DGBaseNodeImpl() {}
};


class DGBaseEdgeImpl 
   : public DirectedGraphEdge<DGBaseNodeImpl,DGBaseEdgeImpl>, 
     public BaseGraphEdge
{
 public:
  DGBaseEdgeImpl( BaseGraphCreate *g, 
                  DGBaseNodeImpl *n1, DGBaseNodeImpl *n2, 
                  BaseGraphCreate::EdgeContent _id)
    : DirectedGraphEdge<DGBaseNodeImpl,DGBaseEdgeImpl>(n1,n2), 
      BaseGraphEdge(g,_id) {}
  ~DGBaseEdgeImpl() {}
};


class DGBaseGraphImpl 
  : public BaseGraphCreateWrap< DirectedGraph<DGBaseNodeImpl,DGBaseEdgeImpl> > 
{ };


// definition of DAGBaseGraphImpl
class DAGBaseEdgeImpl;
class DAGBaseNodeImpl
   : public DAGNode<DAGBaseNodeImpl,DAGBaseEdgeImpl>,
     public BaseGraphNode
{
 public:
  DAGBaseNodeImpl(DAG<DAGBaseNodeImpl,DAGBaseEdgeImpl> *g1, 
                  BaseGraphCreate *g2, BaseGraphCreate::NodeContent _id)
      : DAGNode<DAGBaseNodeImpl,DAGBaseEdgeImpl>(g1), 
          BaseGraphNode(g2,_id) {}
  ~DAGBaseNodeImpl() {}
};
class DAGBaseEdgeImpl
   : public DAGEdge<DAGBaseNodeImpl,DAGBaseEdgeImpl>,
     public BaseGraphEdge
{
 public:
  DAGBaseEdgeImpl( BaseGraphCreate *g, 
                   DAGBaseNodeImpl *n1, DAGBaseNodeImpl *n2, 
                   BaseGraphCreate::EdgeContent _id)
    : DAGEdge<DAGBaseNodeImpl,DAGBaseEdgeImpl>(n1,n2), BaseGraphEdge(g, _id) {}
  ~DAGBaseEdgeImpl() {}
};
class DAGBaseGraphImpl 
    : public BaseGraphCreateWrap< DAG<DAGBaseNodeImpl,DAGBaseEdgeImpl> > 
{
 public:
  void TopoSort( bool reverse = false )
   { impl->TopoSort(reverse); }
};

#endif

