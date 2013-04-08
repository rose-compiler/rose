
#ifndef DIRECTED_GRAPH_IMPL
#define DIRECTED_GRAPH_IMPL

#include <DoublyLinkedList.h>
#include <assert.h>
#include <iostream>
#include "rosedll.h"

class DirectedEdgeInterface
{ public:
   typedef enum {EdgeOut = 0, EdgeIn = 1} EdgeDirection;
};
template <class Node, class Edge> class DirectedGraph;
template <class Node, class Edge> class DirectedGraphEdge;
template <class Node, class Edge>
class ROSE_UTIL_API DirectedGraphNode 
{ 
   DoublyLinkedListWrap <Edge*> edges[2];
   DoublyLinkedEntryWrap<Node*> *entry;
   DirectedGraph<Node,Edge> *graph;  
 public:
   typedef DirectedEdgeInterface::EdgeDirection EdgeDirection;
   typedef typename DoublyLinkedListWrap<Edge*>::iterator EdgeIterator;

   DirectedGraphNode( DirectedGraph<Node,Edge> *g);
   virtual ~DirectedGraphNode();
   EdgeIterator GetEdgeIterator( EdgeDirection dir) const 
       { return EdgeIterator(edges[dir]); }
   unsigned NumberOfEdges( EdgeDirection dir) 
             { return edges[dir].NumberOfEntries(); }
   DirectedGraph<Node,Edge> * GetGraph() const { return graph; }

   void SortEdges( EdgeDirection dir, MapObject<Edge*, int>& f)
      { edges[dir].Sort(f); }
   void SortEdges(  EdgeDirection dir, CompareObject<Edge*> & f)
      { edges[dir].Sort(f); }

 friend class DirectedGraphEdge<Node,Edge>;
};

template <class Node, class Edge>
class ROSE_UTIL_API DirectedGraphEdge
{
  Node* nodes[2];
  DoublyLinkedEntryWrap<Edge*> *entries[2];
 public:
  typedef DirectedEdgeInterface::EdgeDirection EdgeDirection;
  DirectedGraphEdge( Node *_src, Node *_snk) ;
  virtual ~DirectedGraphEdge() ;
  Node* EndPoint( EdgeDirection dir) const { return nodes[dir]; }
  void MoveEndPoint(Node *n, EdgeDirection dir);
};

template <class NodeImpl, class EdgeImpl>
class DirectedGraph : public DirectedEdgeInterface
{
  DoublyLinkedListWrap <NodeImpl*> nodes;
 public:
  typedef DirectedEdgeInterface::EdgeDirection EdgeDirection;
  typedef NodeImpl Node;
  typedef EdgeImpl Edge;
  typedef typename DoublyLinkedListWrap<Node*>::iterator NodeIterator;
  typedef typename DirectedGraphNode<Node,Edge>::EdgeIterator EdgeIterator;

  DirectedGraph() {}
  virtual ~DirectedGraph() 
   { NodeIterator p(nodes); 
     while (!p.ReachEnd()) {
        Node *n = *p;
        ++p;
        delete n;
     }
   }
  NodeIterator GetNodeIterator() const { return NodeIterator(nodes); }
  EdgeIterator GetNodeEdgeIterator(const Node* n, EdgeDirection d) const
       { return n->GetEdgeIterator(d); }
  Node* GetEdgeEndPoint( const Edge* e, EdgeDirection d)
     { return e->EndPoint(d); }
  bool ContainNode( const Node* n) const
    { return n->GetGraph() == this; }
  bool ContainEdge( const Edge* e) const
    { return e->EndPoint(EdgeOut)->GetGraph() == this; }
  unsigned NumberOfNodes() const { return nodes.NumberOfEntries(); }
  
  void SortNodes( MapObject<Node*, int>& f) { nodes.Sort(f); }
  void SortNodes( CompareObject<Node*>& f) { nodes.Sort(f); } 

 friend class DirectedGraphNode<Node, Edge>;
};

#endif

