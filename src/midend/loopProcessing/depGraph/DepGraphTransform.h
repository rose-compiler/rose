#ifndef TRANSFORM_DEPGRAPH
#define TRANSFORM_DEPGRAPH

#include <GraphUpdate.h>
#include <DomainInfo.h>
#include <DepInfoUpdate.h>
#include <DepGraph.h>

inline DepDirection EdgeDir2DepDir( GraphAccess::EdgeDirection dir)
{
  switch (dir) {
  case GraphAccess::EdgeOut : return DEP_SRC;
  case GraphAccess::EdgeIn : return DEP_SINK;
  case GraphAccess::BiEdge: return DEP_SRC_SINK;
  default: assert (!"Case not handled");
  } 
}

template <class Graph, class DepInfoUpdate>
class UpdateDepEdgeInfo {
  DepInfoUpdate T;
  Graph *graph;
 public:
  UpdateDepEdgeInfo( Graph* g, const DepInfoUpdate& _T) : T(_T), graph(g) {}
  void operator() ( typename Graph::Edge *e, GraphAccess::EdgeDirection dir)
   {  if (! UpdateDepInfo<DepInfoUpdate>(T)( e->GetInfo(), EdgeDir2DepDir(dir)) )
          graph->DeleteEdge(e); 
   }
};

template <class Graph>
void CopySplitNodeEdge 
( Graph *graph, typename Graph::Node* orig, typename Graph::Node* split,
                        const DomainCond &splitCond)
{
  CopyEachNodeEdge<Graph>()(graph, orig, split);
  typedef UpdateDepEdgeInfo<Graph,DepInfoRestrictDomain> InfoOp;
  UpdateEachNodeEdge (graph,split, InfoOp(graph,DepInfoRestrictDomain(splitCond)) );
}

template <class Graph>
void UpdateSplitNodeEdge ( Graph *c, typename Graph::Node* split, 
                        const DomainCond &splitCond)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoRestrictDomain> InfoOp;
  UpdateEachNodeEdge(c,split, InfoOp(c,DepInfoRestrictDomain(splitCond)) );
}

template <class Graph>
void DepGraphNodeRestrictDomain
( Graph *graph, typename Graph::Node* node, const DomainCond &cond)
{
 typedef UpdateDepEdgeInfo<Graph,DepInfoRestrictDomain> InfoOp;
 UpdateEachNodeEdge ( graph, node, InfoOp(graph,DepInfoRestrictDomain(cond)) );
}

template <class Graph>
void FuseDepGraphNode
( Graph* graph, typename Graph::Node* orig, typename Graph::Node* split)
{
  MoveEachNodeEdge<Graph>()( graph, split, orig);
  graph->DeleteNode(split);
}


template <class Graph, class SelectNode>
void DepGraphInsertLoop (Graph* graph, SelectNode nodes, int level)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoInsertLoop> InfoOp;
  UpdateEachNodeEdge( graph, nodes, InfoOp(graph,DepInfoInsertLoop(level)) );
}

template <class Graph>
void DepGraphInsertLoop (Graph* graph, typename Graph::Node* n, int level)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoInsertLoop> InfoOp;
  UpdateEachNodeEdge( graph, n, InfoOp(graph,DepInfoInsertLoop(level)) );
}

template <class Graph>
void DepGraphNodeSwapLoop
(Graph* graph, typename Graph::Node* node, int level1, int level2)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoSwapLoop> InfoOp;
  UpdateEachNodeEdge( graph, node, InfoOp(graph,DepInfoSwapLoop(level1,level2)) );
}


template <class Graph>
void DepGraphNodeRemoveLoop (Graph* graph, typename Graph::Node* node, int level)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoRemoveLoop> InfoOp;
  UpdateEachNodeEdge( graph, node, InfoOp(graph,DepInfoRemoveLoop(level)) );
}

template <class Graph>
void DepGraphNodeMergeLoop (Graph* graph, typename Graph::Node* node,
                           int desc, int src)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoMergeLoop> InfoOp;
  UpdateEachNodeEdge ( graph, node, InfoOp(graph, DepInfoMergeLoop(desc,src)) );
}

template <class Graph>
void DepGraphNodeAlignLoop (Graph* graph, typename Graph::Node* node,
                           int level, int align)
{
  if (align != 0) {
    typedef UpdateDepEdgeInfo<Graph,DepInfoAlignLoop> InfoOp;
    UpdateEachNodeEdge ( graph, node, InfoOp(graph,DepInfoAlignLoop(level,-align)) );
  }
}


#endif
