
#include <DirectedGraph.h>

template <class Node, class Edge>
DirectedGraphEdge<Node,Edge>::DirectedGraphEdge( Node *_src, Node *_snk)
   { 
          nodes[0] = _src; nodes[1] = _snk;
          for (int i = 0; i < 2; ++i)
                entries[i] = nodes[i]->edges[i].AppendLast( static_cast<Edge*>(this) );
   }

template <class Node, class Edge>
DirectedGraphEdge<Node,Edge>::~DirectedGraphEdge()
 {
        for (int i = 0; i < 2; ++i)
                nodes[i]->edges[i].Delete(entries[i]);
 }

template <class Node, class Edge>
void DirectedGraphEdge<Node,Edge>:: MoveEndPoint(Node *n, EdgeDirection dir)
  { 
      nodes[dir]->edges[dir].Delete(entries[dir]);
      nodes[dir] = n;
      entries[dir] = n->edges[dir].AppendLast( static_cast<Edge*>(this) );
  }


template <class Node, class Edge>
DirectedGraphNode<Node, Edge> :: 
DirectedGraphNode( DirectedGraph<Node,Edge> *g)
 : graph(g) 
{ 
  entry = g->nodes.AppendLast( static_cast<Node*>(this) ); 
}

template <class Node, class Edge>
DirectedGraphNode<Node, Edge> :: ~DirectedGraphNode()
{ 
   for (int i = 0; i < 2; ++i) {
      EdgeIterator p(edges[i]);
      while (!p.ReachEnd()) {
        DirectedGraphEdge<Node,Edge> *e = *p;
        ++p;
        delete e;
      }
   }
   graph->nodes.Delete(entry); 
}

