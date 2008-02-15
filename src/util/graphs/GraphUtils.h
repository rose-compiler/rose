
#ifndef GRAPH_UTILS
#define GRAPH_UTILS

#include <IteratorTmpl.h>
#include <IteratorCompound.h>
#include <string>
#include <GraphInterface.h>


template<class Graph>
class GraphNodeGetEdgeIterator
{
  const Graph *graph;
  typename Graph::EdgeDirection dir;
 public:
  GraphNodeGetEdgeIterator( const Graph *g, typename Graph::EdgeDirection d)
    : graph(g), dir(d) {}
  typename Graph::EdgeIterator operator() (typename Graph::Node* n)
   { return graph->GetNodeEdgeIterator(n, dir); }
};

template <class Graph>
class GraphGetEdgeIterator
{ public:
    typename Graph::EdgeIterator operator()(const Graph* g)
    { 
      typedef GraphNodeGetEdgeIterator<Graph> GraphNodeGetEdge;
      typedef MultiCrossIterator<typename Graph::Edge*,typename Graph::NodeIterator,
                  typename Graph::EdgeIterator, GraphNodeGetEdge> 
           GraphEdgeMultiNodeIterator;

       GraphEdgeMultiNodeIterator iter(g->GetNodeIterator(),
                                  GraphNodeGetEdge(g, GraphAccess::EdgeOut));
       return new Iterator2ImplTemplate<GraphEdge*, typename Graph::Edge*, 
                                   GraphEdgeMultiNodeIterator>(iter);
    }
};

template<class Graph>
class GraphGetEdgeEndPoint
{
  const Graph *graph;
  typename Graph::EdgeDirection dir;
 public:
  GraphGetEdgeEndPoint( const Graph *g, typename Graph::EdgeDirection d)
   : graph(g), dir(d) {}
  typename Graph::Node* operator() (const typename Graph::Edge *e) const
   { return graph->GetEdgeEndPoint(e, dir); }
};

template <class Graph>
class GraphSelectEdgeEndPoint
{
  const Graph *graph;
  const typename Graph::Node *node;
  typename Graph::EdgeDirection dir;
 public:
  GraphSelectEdgeEndPoint( const Graph *g, const typename Graph::Node *n, 
                      typename Graph::EdgeDirection d)
   : graph(g), node(n), dir(d) {}
//Boolean operator() (const typename Graph::Edge *e) const
  int operator() (const typename Graph::Edge *e) const
   { return graph->GetEdgeEndPoint(e, dir) == node; }
};

template <class Graph>
class GraphGetCrossEdgeIterator
{ public:
   typename Graph::EdgeIterator 
     operator()(const Graph* g, const typename Graph::Node *src, 
                const typename Graph::Node *snk) const
   {
    typedef SelectIterator<typename Graph::Edge*, typename Graph::Edge*, 
                           typename Graph::EdgeIterator,
                           GraphSelectEdgeEndPoint<Graph> >
          EdgeSelectIterator;
    EdgeSelectIterator iter( g->GetNodeEdgeIterator(src,GraphAccess::EdgeOut),
                           GraphSelectEdgeEndPoint<Graph>(g, snk, GraphAccess::EdgeIn));
    return new Iterator2ImplTemplate<GraphEdge*,typename Graph::Edge*, EdgeSelectIterator>
                  (iter);
  }
};

template <class Graph>
class GraphGetCrossEdge
{ public:
  typename Graph::Edge* operator()( const Graph* g, const typename Graph::Node *src, 
                                    const typename Graph::Node *snk) const
       { typename Graph::EdgeIterator p = GraphGetCrossEdgeIterator<Graph>()(g,src,snk);
         return p.ReachEnd()? 0 : p.Current(); 
       }
};

template <class Graph>
class GraphGetNodePredecessors
{ public:
  typename Graph::NodeIterator operator() 
        ( const Graph* g, const typename Graph::Node* n) const
       {
        typedef SingleCrossIterator<typename Graph::Edge*, typename Graph::Node*, 
                 typename Graph::EdgeIterator, GraphGetEdgeEndPoint<Graph> > 
           PredIterator;
        PredIterator iter(g->GetNodeEdgeIterator(n, GraphAccess::EdgeIn), 
                          GraphGetEdgeEndPoint<Graph>(g, GraphAccess::EdgeOut));
        return new Iterator2ImplTemplate<GraphNode*, typename Graph::Node*,PredIterator>(iter);
       }
};

template <class Graph>
class GraphGetNodeSuccessors
{ public:
  typename Graph::NodeIterator operator() 
         ( const Graph* g, const typename Graph::Node* n) const
       {
        typedef SingleCrossIterator<typename Graph::Edge*, typename Graph::Node*, 
                typename Graph::EdgeIterator, GraphGetEdgeEndPoint<Graph> > 
             SuccIterator;
        SuccIterator iter(g->GetNodeEdgeIterator(n, GraphAccess::EdgeOut), 
                          GraphGetEdgeEndPoint<Graph>(g, GraphAccess::EdgeIn));
        return new Iterator2ImplTemplate<GraphNode*, typename Graph::Node*,SuccIterator>(iter);
       }
};

template <class Graph, class Collect>
class GraphGetNodeReachable 
{ public:
   void operator()(const Graph* g, const typename Graph::Node* n, 
                   typename Graph::EdgeDirection dir,
                   Collect& collect)
   {
     typename Graph::EdgeDirection dir1 = GraphAccess::Reverse(dir);
     if (collect(n)) {
        for (typename Graph::EdgeIterator edgep = g->GetNodeEdgeIterator( n, dir);
             !edgep.ReachEnd(); ++edgep) {
           typename Graph::Edge *e = edgep.Current();
           typename Graph::Node *n1 = g->GetEdgeEndPoint( e, dir1);
           operator()( g,n1, dir, collect);
        }
     }
   }
};
#endif
