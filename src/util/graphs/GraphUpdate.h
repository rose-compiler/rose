#ifndef GRAPH_UPDATE_H
#define GRAPH_UPDATE_H

template <class Graph>
class SelectSingleNode {
   Graph *g;
   typename Graph::Node* orig, *cur;
 public:
   typedef SelectSingleNode Iterator;

   SelectSingleNode(Graph* _g, typename Graph::Node* o) 
          : g(_g), orig(o),cur(o) {}
   bool operator() (typename Graph::Node* n) const
        { return orig == n; }
   void Reset() { cur = orig; }
   typename Graph::Node* Current() const { return cur; } 
   typename Graph::Node* operator*() const { return Current(); }
   void Advance()  { cur = 0; }
   void operator ++() { Advance(); }
   void operator ++(int) { Advance(); }
   bool ReachEnd() const { return cur == 0; }
   Iterator GetIterator() const { return *this; }   
};

template <class Graph, class SelectNode, class EdgeOp>
void UpdateEachNodeEdge( Graph* graph, SelectNode nodes, EdgeOp op)
    { 
      typename SelectNode::Iterator nodep = nodes.GetIterator();
      for (; !nodep.ReachEnd(); ++nodep) { 
         typename Graph::Node* orig = *nodep;
         typename Graph::EdgeIterator iter = 
                 graph->GetNodeEdgeIterator(orig, GraphAccess::EdgeOut);
         while (!iter.ReachEnd()) {
            typename Graph::Edge *e = iter.Current();
            iter.Advance();
            if (nodes(graph->GetEdgeEndPoint(e,GraphAccess::EdgeIn)) )
              op(e, GraphAccess::BiEdge);
            else
              op(e, GraphAccess::EdgeOut);
        }
        iter = graph->GetNodeEdgeIterator(orig, GraphAccess::EdgeIn);
        while (!iter.ReachEnd()) {
            typename Graph::Edge *e = iter.Current(); 
            iter.Advance();
            if (!nodes(graph->GetEdgeEndPoint(e, GraphAccess::EdgeOut)) ) 
              op(e, GraphAccess::EdgeIn);
        }
      }
    }
template <class Graph, class EdgeOp>
void UpdateEachNodeEdge ( Graph* graph, typename Graph::Node* orig, EdgeOp op)
{
  SelectSingleNode<Graph> sel(graph, orig);
  UpdateEachNodeEdge(graph, sel, op);
}

template <class Graph, class ModifyEdgeInfo>
class UpdateEdgeInfo {
  ModifyEdgeInfo T;
  Graph *graph;
 public:
  UpdateEdgeInfo( Graph* g, ModifyEdgeInfo _T) : graph(g), T(_T) {}
  void operator() ( typename Graph::Edge *e, GraphAccess::EdgeDirection dir)
   {  if (! T( e->GetInfo(), dir)) 
          graph->DeleteEdge(e); }
};

template <class Graph>
class SingleNodeCopyEdge
{
  Graph *graph;
  typename Graph::Node *node;
 public:
  SingleNodeCopyEdge( Graph *g, typename Graph::Node* n)
    : graph(g), node(n)  {}
  void operator() (const typename Graph::Edge *e, GraphAccess::EdgeDirection dir)
    {
      if (dir & GraphAccess::EdgeOut) {
        typename Graph::Node *n2 = graph->GetEdgeEndPoint(e, GraphAccess::EdgeIn);
        graph->CreateEdge(node, n2, e->GetInfo());
      }
      if (dir & GraphAccess::EdgeIn) {
        typename Graph::Node *n2 = graph->GetEdgeEndPoint(e, GraphAccess::EdgeOut);
        graph->CreateEdge(n2,node, e->GetInfo());
      }
      if (dir == GraphAccess::BiEdge)
        graph->CreateEdge(node,node,e->GetInfo());
    }
};

template <class Graph>
class SingleNodeMoveEdge
{
  Graph* graph;
  typename Graph::Node* node;
 public:
  SingleNodeMoveEdge( Graph *g, typename Graph::Node* n) : graph(g), node(n) {}
  void operator() (typename Graph::Edge *e, GraphAccess::EdgeDirection dir)
    { if (dir & GraphAccess::EdgeOut)
         graph->MoveEdgeEndPoint(e, GraphAccess::EdgeOut, node); 
      if (dir & GraphAccess::EdgeIn)
         graph->MoveEdgeEndPoint(e, GraphAccess::EdgeIn, node);
    }
};

template <class Graph>
class CopyEachNodeEdge
{ public:
   void operator()( Graph* graph, typename Graph::Node* orig, typename Graph::Node* des)
    { typedef SingleNodeCopyEdge<Graph> EdgeOp;
      UpdateEachNodeEdge (graph, orig, EdgeOp(graph,des));
    }
};

template <class Graph>
class MoveEachNodeEdge
{ public:
   void operator()( Graph* graph, typename Graph::Node* orig, typename Graph::Node* des)
    { typedef SingleNodeMoveEdge<Graph> EdgeOp;
      UpdateEachNodeEdge (graph, orig, EdgeOp(graph,des));
    }
};

#endif
