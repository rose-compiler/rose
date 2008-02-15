
#ifndef DG_BASE_GRAPH_IMPL
#define DG_BASE_GRAPH_IMPL

#include <assert.h>
#include <BaseGraphCreate.h>
#include <DAG.h>
#include <sstream>

// definition of DGBaseGraphImpl
class DGBaseEdgeImpl;
class DGBaseNodeImpl;
class DGBaseNodeImpl 
   : public DirectedGraphNode<DGBaseNodeImpl,DGBaseEdgeImpl>, 
     public BaseGraphCreate::Node
{
 public:
  DGBaseNodeImpl( DirectedGraph<DGBaseNodeImpl,DGBaseEdgeImpl> *g1, BaseGraphCreate *g2, 
                  GraphNode *_id)
    : DirectedGraphNode<DGBaseNodeImpl,DGBaseEdgeImpl>(g1), BaseGraphCreate::Node(g2,_id) {}
  ~DGBaseNodeImpl() {}
};
class DGBaseEdgeImpl 
   : public DirectedGraphEdge<DGBaseNodeImpl,DGBaseEdgeImpl>, 
     public BaseGraphCreate::Edge
{
 public:
  DGBaseEdgeImpl( BaseGraphCreate *g, 
                  DGBaseNodeImpl *n1, DGBaseNodeImpl *n2, GraphEdge *_id)
    : DirectedGraphEdge<DGBaseNodeImpl,DGBaseEdgeImpl>(n1,n2), BaseGraphCreate::Edge(g,_id) {}
  ~DGBaseEdgeImpl() {}
};
class DGBaseGraphImpl 
  : public BaseGraphCreateTemplate<DGBaseNodeImpl, DGBaseEdgeImpl, 
                             DirectedGraph<DGBaseNodeImpl,DGBaseEdgeImpl> > 
{
};


// definition of DAGBaseGraphImpl
class DAGBaseEdgeImpl;
class DAGBaseNodeImpl;
class DAGBaseNodeImpl
   : public DAGNode<DAGBaseNodeImpl,DAGBaseEdgeImpl>,
     public BaseGraphCreate::Node
{
 public:
  DAGBaseNodeImpl(DAG<DAGBaseNodeImpl,DAGBaseEdgeImpl> *g1, BaseGraphCreate *g2, GraphNode *_id)
      : DAGNode<DAGBaseNodeImpl,DAGBaseEdgeImpl>(g1), BaseGraphCreate::Node(g2,_id) {}
  ~DAGBaseNodeImpl() {}
  std::string ToString() const 
   {
     std::stringstream res;
     res << BaseGraphCreate::Node::ToString() << " : " << "topo order index = " << TopoOrderIndex();
     return res.str();
   }
};
class DAGBaseEdgeImpl
   : public DAGEdge<DAGBaseNodeImpl,DAGBaseEdgeImpl>,
     public BaseGraphCreate::Edge
{
 public:
  DAGBaseEdgeImpl( BaseGraphCreate *g, 
                   DAGBaseNodeImpl *n1, DAGBaseNodeImpl *n2, GraphEdge *_id)
    : DAGEdge<DAGBaseNodeImpl,DAGBaseEdgeImpl>(n1,n2), BaseGraphCreate::Edge(g, _id) {}
  ~DAGBaseEdgeImpl() {}

  std::string ToString() const 
  {  std::stringstream res;
     res << BaseGraphCreate::Edge::ToString();
     if (IsBackEdge())
        res << " : is back edge ";
     return res.str();
  }

 friend class DAGBaseGraphImpl;
};
class DAGBaseGraphImpl 
    : public BaseGraphCreateTemplate<DAGBaseNodeImpl, DAGBaseEdgeImpl, 
                               DAG<DAGBaseNodeImpl,DAGBaseEdgeImpl> > 
{
 public:
//void TopoSort( Boolean reverse = false )
  void TopoSort( int reverse = false )
   { impl->TopoSort(reverse); }
};


#endif

