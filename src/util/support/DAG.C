#include <DAG.h>

template <class Node, class Edge>
DAGNode<Node,Edge> :: DAGNode(DAG<Node,Edge> *dag)
  : DirectedGraphNode<Node,Edge>(dag) 
{ visitNo = 0; ordNo = dag->NumberOfNodes(); }

template <class Node, class Edge>
DAGNode<Node,Edge> :: ~DAGNode()
{ 
  DAG<Node,Edge>* dag = GetGraph();
  if (ordNo != dag->NumberOfNodes())
     dag->ordered = false;
}

template <class Node, class Edge>
DAG<Node,Edge>*
DAGNode<Node,Edge> :: GetGraph()  const
   { return static_cast< DAG<Node,Edge>* > 
            (DirectedGraphNode<Node,Edge>::GetGraph()); }

template <class Node, class Edge>
unsigned DAGNode<Node,Edge> :: TopoOrderIndex() const
{ 
#ifndef NDEBUG
  DAG<Node,Edge>* dag = GetGraph();
  assert(dag->ordered);
#endif
   return ordNo;
}

template <class Node, class Edge>
DAGEdge<Node,Edge> ::  DAGEdge( Node *_src, Node *_snk)
  : DirectedGraphEdge<Node,Edge>(_src, _snk), isBackEdge(false)
  { 
     DAGNode<Node,Edge>* src = _src;
     if ( src->GetGraph()->ordered && !ValidTopoOrder())
         src->GetGraph()->ordered = false; 
  }

template <class Node, class Edge>
void DAGEdge<Node,Edge>::
MoveEndPoint(Node *n, EdgeDirection dir)
 { DirectedGraphEdge<Node,Edge>:: MoveEndPoint(n, dir);
   if ( n->GetGraph()->ordered) {
      if (ValidTopoOrder())
        isBackEdge = false;
      else if (!isBackEdge)
        n->GetGraph()->ordered = false; 
   }
}
template <class Node, class Edge>
bool DAGEdge<Node,Edge>:: IsBackEdge() const
{
#ifndef NDEBUG
   DAG<Node,Edge>* dag = EndPoint( DirectedEdgeInterface::EdgeOut)->GetGraph();
   assert(dag->ordered);
#endif
   return isBackEdge;
}

