#ifndef GRAPH_UPDATE_H
#define GRAPH_UPDATE_H

#include <GraphAccess.h>

template <class Graph>
class SelectSingleNode {
   Graph *g;
   typename Graph::Node* orig, *cur;
 public:
   typedef SelectSingleNode const_iterator;

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
   const_iterator begin() const { return *this; }   
};

template <class Graph, class SelectNode, class EdgeOp>
void UpdateEachNodeEdge( Graph* graph, SelectNode nodes, EdgeOp op)
    { 
      typename SelectNode::const_iterator nodep = nodes.begin();
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
template <class Graph, class EdgeOp, class Node>
void UpdateNodeEdge ( Graph* graph, Node* orig, EdgeOp op)
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

template <class Graph, class Node>
void CopyNodeEdge( Graph* graph, Node* orig, Node* des)
    { typedef SingleNodeCopyEdge<Graph> EdgeOp;
      UpdateNodeEdge (graph, orig, EdgeOp(graph,des));
    }

template <class Graph, class Node>
void MoveNodeEdge( Graph* graph, Node* orig, Node* des)
    { typedef SingleNodeMoveEdge<Graph> EdgeOp;
      UpdateNodeEdge (graph, orig, EdgeOp(graph,des));
    }

 
 
// DQ (3/21/2006): Contribution for graph filtering from Andreas (using Functor interface)
/*************************************************************************************************
 * The function
 *   void filterGraph(_GraphStructure& graph, _NodePredicate _nodePred)
 * will filter out all nodes which satisfy the criteria which makes
_NodePredicate return 'true'.
 * _NodePredicate is any function object that takes a
_GraphStructure::Node* as a paramater and
 * returns a bool. In order for this to work a function
_GraphStructure::DeleteNode must be implemented
 * in order to Delete the nodes. This exact function is implemented as
protected in IDGraphCreateTemplate
 * so it should be easy.
 *************************************************************************************************/
template<typename _GraphStructure, typename _NodePredicate>
void filterGraph(_GraphStructure& graph, _NodePredicate _nodePred){
     typename _GraphStructure::NodeIterator it1 =  graph. GetNodeIterator();
     while(!it1.ReachEnd()){
      typename _GraphStructure::Node* x1 = *it1;
#if !USE_ROSE
   // DQ (11/3/2011): EDG compilains about this (but GNU allowed it, I think that EDG might be correct.
   // since it see the instantiated template differently.  But since we are only trying to compile ROSE 
   // with ROSE (using the new EDG 4.3 front-end as a tests) we can just skip this case for now.
      ROSE_ASSERT(x1!=NULL);
#endif
      _nodePred(x1);

      it1++;

      if(_nodePred(x1)==true)
         graph.DeleteNode(x1);

     }
};

#endif
