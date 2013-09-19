#ifndef DirectedAcyclicGraph_h
#define DirectedAcyclicGraph_h

#include <stdlib.h>

#include <DirectedGraph.h>
#include <PtrSet.h>
#include "rosedll.h"

template <class Node, class Edge> class DAG;
template <class Node, class Edge>
class ROSE_UTIL_API DAGNode : public DirectedGraphNode<Node, Edge>
{
  unsigned ordNo, visitNo;
 public:
  DAGNode(DAG<Node,Edge> *dag);
  virtual ~DAGNode();
  
  DAG<Node,Edge>* GetGraph() const;
  unsigned TopoOrderIndex() const;
 friend class DAG<Node,Edge>;
};

template <class Node, class Edge>
class ROSE_UTIL_API DAGEdge : public DirectedGraphEdge<Node,Edge>
{
  bool isBackEdge;
  bool ValidTopoOrder() 
     { return EndPoint(DirectedEdgeInterface::EdgeOut)->TopoOrderIndex() 
             < EndPoint(DirectedEdgeInterface::EdgeIn)->TopoOrderIndex();
     }
 public:
  typedef typename DirectedGraphEdge<Node,Edge>::EdgeDirection EdgeDirection;
  DAGEdge( Node *_src, Node *_snk);
  virtual ~DAGEdge() {}
  void MoveEndPoint(Node *n, EdgeDirection dir);
  bool IsBackEdge() const;
  DirectedGraphEdge<Node,Edge>::EndPoint;
 friend class DAG<Node,Edge>;
};

template <class Node, class Edge>
class ROSE_UTIL_API DAG  : public DirectedGraph<Node,Edge>
{
 public:
  typedef enum {NON_SORT, TOPO_SORT, R_TOPO_SORT} SortType;
  typedef typename DirectedGraph<Node,Edge>::EdgeDirection EdgeDirection;
  typedef typename DirectedGraph<Node,Edge>::EdgeIterator EdgeIterator;
  typedef typename DirectedGraph<Node,Edge>::NodeIterator NodeIterator;

 private:
  bool ordered;
  SortType sort;
  
  class NodeTopoOrderIndex : public MapObject<Node*, int>
  { bool reverse;
    public: 
      NodeTopoOrderIndex( bool r) : reverse(r) {}
      int operator ()( Node* const& n) 
          { 
            return (reverse)? 
                      n->GetGraph()->NumberOfNodes() - n->TopoOrderIndex() 
                     : n->TopoOrderIndex() - 1; 
          }
  };
  class NodeIncomingEdgeLessThan : public CompareObject<Node*>
  { public:
      int operator ()( Node* const& n1, Node* const& n2) 
          { 
            int i1 = n1->NumberOfEdges( DirectedEdgeInterface::EdgeIn);
            int i2 = n2->NumberOfEdges( DirectedEdgeInterface::EdgeIn); 
            if (i1 == i2)
               return 0;
            else 
               return (i1 < i2)? -1 : 1;
          }
   };
  class EdgeOrderLessThan : public CompareObject<Edge*>
  { 
    EdgeDirection dir;
    bool reverse;
   public:
    EdgeOrderLessThan( EdgeDirection d, bool r) : dir(d), reverse(r) {}
    bool operator () ( Edge* const& e1, Edge* const& e2) 
      { 
        int i1 = e1->EndPoint(dir)->TopoOrderIndex();
        int i2 = e2->EndPoint(dir)->TopoOrderIndex();
        if (i1 == i2)
            return 0;
        else if (reverse) 
           return (i1 > i2)? -1 : 1;
        else
           return (i1 < i2)? -1 : 1;   
      }
  };

  int TopoOrderNodes( Node *node)
  {
    int sortIndex = node->ordNo;
    for ( EdgeIterator p = node->GetEdgeIterator( DirectedEdgeInterface::EdgeOut);
          !p.ReachEnd(); ++p) {
      Edge *e = *p;
      Node *snk = e->EndPoint( DirectedEdgeInterface::EdgeIn );
      if ( snk->ordNo != 0) {
         e->isBackEdge = true;
      }
      else {
         ++snk->visitNo;
         if (snk->visitNo == snk->NumberOfEdges( DirectedEdgeInterface::EdgeIn)) { 
            snk->ordNo = sortIndex+1;
            sortIndex = TopoOrderNodes(snk);
         }
      }
    }
    return sortIndex;
  }

 public:

  DAG() : ordered(true), sort(NON_SORT) {}
  virtual ~DAG() {}

  DirectedGraph<Node,Edge>::GetNodeIterator;

  void TopoOrderNodes()
  { if (!ordered) {
      NodeIterator p = GetNodeIterator(); 
      for ( ; !p.ReachEnd(); ++p) {
         Node *node = *p;
         node->ordNo = node->visitNo = 0;
      }
      NodeIncomingEdgeLessThan op;
      DirectedGraph<Node,Edge>::SortNodes( op );
      int sortIndex = 0;
      for ( p.Reset(); !p.ReachEnd(); ++p) {
         Node *node = *p;
         if (node->ordNo == 0) {
            node->ordNo = sortIndex+1;
            sortIndex = TopoOrderNodes(node);
         }
      }
      ordered = true;
    }
  }

  void TopoSort( bool reverse = false )
   {  if (!ordered) {
         TopoOrderNodes();
         sort = NON_SORT;
      }
      if ( (sort == TOPO_SORT && !reverse) || (sort == R_TOPO_SORT && reverse))
         return;
      NodeTopoOrderIndex nodeOp( reverse ) ;
      DirectedGraph<Node,Edge>::SortNodes( nodeOp);
      sort = (reverse)? R_TOPO_SORT : TOPO_SORT;
   }

  void SortNodes( MapObject<Node*, int>& f) 
        { DirectedGraph<Node,Edge>::SortNodes(f); sort = NON_SORT; }
  void SortNodes( CompareObject<Node*>& f) 
        { DirectedGraph<Node,Edge>::SortNodes(f); sort = NON_SORT; }

 friend class DAGNode<Node,Edge>; 
 friend class DAGEdge<Node,Edge>;
};

#endif
    
