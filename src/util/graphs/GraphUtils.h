
#ifndef GRAPH_UTILS
#define GRAPH_UTILS

#include <IteratorTmpl.h>
#include <GraphUtils.h>
#include <string>


template<class Graph>
class GraphEdgeIterator : public IteratorImpl<typename Graph::Edge*>
{
  const Graph *graph;
  typename Graph::NodeIterator nodep;
  typename Graph::EdgeIterator edgep;
  void SetIterator()
   {
     for ( ; !nodep.ReachEnd(); ++nodep) {
       edgep = graph->GetNodeEdgeIterator(*nodep, GraphAccess::EdgeOut);
       if (!edgep.ReachEnd()) break;
     } 
   }

 public:
  GraphEdgeIterator( const Graph *g) : graph(g) 
     {  
        nodep = g->GetNodeIterator(); 
        SetIterator();
     }
  ~GraphEdgeIterator() {}

  void Advance()
   {
     edgep.Advance();
     if (edgep.ReachEnd()) {
       nodep.Advance();
       SetIterator();
     }
   }
  void Reset() { nodep.Reset(); SetIterator(); }
  typename Graph::Edge* Current()  const { return edgep.Current(); }
  bool ReachEnd() const { return nodep.ReachEnd() && edgep.ReachEnd(); }
  IteratorImpl<typename Graph::Edge*>* Clone() const 
         { return new GraphEdgeIterator<Graph>(*this); }
};

template <class Graph>
class GraphCrossEdgeIterator : public IteratorImpl<typename Graph::Edge*>
{ 
  const Graph *graph;
  typename Graph::EdgeIterator edgep;
  const typename Graph::Node *snk;
  void SetIterator()
   {
     while (!edgep.ReachEnd() && 
            graph->GetEdgeEndPoint(*edgep, GraphAccess::EdgeIn) != snk) 
       edgep.Advance();
   }

 public:
  GraphCrossEdgeIterator( const Graph* g, const typename Graph::Node *_src, 
                                    const typename Graph::Node *_snk) 
     : graph(g), snk(_snk)
   { 
     edgep = g->GetNodeEdgeIterator(_src, GraphAccess::EdgeOut); 
     SetIterator();
   }
  ~GraphCrossEdgeIterator() {}

  void Advance()
   { edgep.Advance(); SetIterator(); }
  void Reset() { edgep.Reset(); SetIterator(); }
  typename Graph::Edge* Current()  const { return edgep.Current(); }
  bool ReachEnd() const { return edgep.ReachEnd(); }
  IteratorImpl<typename Graph::Edge*>* Clone() const 
            { return new GraphCrossEdgeIterator(*this); }
};

template <class Graph>
class GraphNodePredecessorIterator : public IteratorImpl<typename Graph::Node*>
{ 
   const Graph* graph;
   typename Graph::EdgeIterator edgep;
  public:
   GraphNodePredecessorIterator( const Graph* g, const typename Graph::Node* n) 
     : graph(g) 
       { edgep = graph->GetNodeEdgeIterator(n, GraphAccess::EdgeIn); }
  ~GraphNodePredecessorIterator() {}

  void Advance() { edgep.Advance(); }
  void Reset() { edgep.Reset(); }
  typename Graph::Node* Current()  const 
     { return graph->GetEdgeEndPoint(edgep.Current(), GraphAccess::EdgeOut); }
  bool ReachEnd() const { return edgep.ReachEnd(); }
  IteratorImpl<typename Graph::Node*>* Clone() const 
      { return new GraphNodePredecessorIterator(*this); }
};

template <class Graph>
class GraphNodeSuccessorIterator : public IteratorImpl<typename Graph::Node*>
{ 
   const Graph* graph;
   typename Graph::EdgeIterator edgep;
  public:
   GraphNodeSuccessorIterator( const Graph* g, const typename Graph::Node* n) 
     : graph(g) 
       { edgep = g->GetNodeEdgeIterator(n, GraphAccess::EdgeOut); }
  ~GraphNodeSuccessorIterator() {}

  void Advance() { edgep.Advance(); }
  void Reset() { edgep.Reset(); }
  typename Graph::Node* Current()  const 
     { return graph->GetEdgeEndPoint(edgep.Current(), GraphAccess::EdgeIn); }
  bool ReachEnd() const { return edgep.ReachEnd(); }
  IteratorImpl<typename Graph::Node*>* Clone() const 
          { return new GraphNodeSuccessorIterator(*this); }
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
