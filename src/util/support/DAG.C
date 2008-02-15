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
{ DAG<Node,Edge>* dag = GetGraph();
  assert(dag->ordered);
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
// Boolean DAGEdge<Node,Edge>:: IsBackEdge() const
int DAGEdge<Node,Edge>:: IsBackEdge() const
{
// DQ (8/2/2005): This was a suggested fix that appeared to not be required 
// the assert(false) was put in and is not required.
// pmp070605  (suggested fix)
// assert(false);
// DAG<Node,Edge>* dag = EndPoint( DirectedEdgeInterface::EdgeOut)->GetGraph();
// assert(dag->ordered);

   DAG<Node,Edge>* dag = EndPoint( DirectedEdgeInterface::EdgeOut)->GetGraph();
   assert(dag->ordered);
   return isBackEdge;
}

