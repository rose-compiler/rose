#ifndef TRANSFORM_DEPGRAPH
#define TRANSFORM_DEPGRAPH

#include <GraphUpdate.h>
#include <DomainInfo.h>
#include <DepInfoUpdate.h>
#include <DepGraph.h>
#include <assert.h>

inline DepDirection EdgeDir2DepDir( GraphAccess::EdgeDirection dir)
{
  switch (dir) {
  case GraphAccess::EdgeOut : return DEP_SRC;
  case GraphAccess::EdgeIn : return DEP_SINK;
  case GraphAccess::BiEdge: return DEP_SRC_SINK;
  default: assert(false);
  } 
        // tps (12/09/2009) : Removed warning : not all control paths return a value
  assert(false);
  return DEP_SRC;
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

template <class Graph, class Node>
void CopySplitNodeEdge 
( Graph *graph, Node* orig, Node* split, const DomainCond &splitCond)
{
  CopyNodeEdge(graph, orig, split);
  typedef UpdateDepEdgeInfo<Graph,DepInfoRestrictDomain> InfoOp;
  UpdateNodeEdge (graph,split, InfoOp(graph,DepInfoRestrictDomain(splitCond)) );
}

template <class Graph, class Node>
void UpdateSplitNodeEdge ( Graph *c, Node* split, const DomainCond &splitCond)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoRestrictDomain> InfoOp;
  UpdateNodeEdge(c,split, InfoOp(c,DepInfoRestrictDomain(splitCond)) );
}

template <class Graph, class Node>
void DepGraphNodeRestrictDomain
( Graph *graph, Node* node, const DomainCond &cond)
{
 typedef UpdateDepEdgeInfo<Graph,DepInfoRestrictDomain> InfoOp;
 UpdateNodeEdge ( graph, node, InfoOp(graph,DepInfoRestrictDomain(cond)) );
}

template <class Graph, class Node>
void FuseDepGraphNode ( Graph* graph, Node* orig, Node* split)
{
  MoveNodeEdge( graph, split, orig);
  graph->DeleteNode(split);
}


template <class Graph, class SelectNode>
void DepGraphInsertLoop (Graph* graph, SelectNode nodes, int level)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoInsertLoop> InfoOp;
  UpdateEachNodeEdge( graph, nodes, InfoOp(graph,DepInfoInsertLoop(level)) );
}

template <class Graph, class Node>
void DepGraphNodeInsertLoop (Graph* graph, Node* n, int level)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoInsertLoop> InfoOp;
  UpdateNodeEdge( graph, n, InfoOp(graph,DepInfoInsertLoop(level)) );
}

template <class Graph, class Node>
void DepGraphNodeSwapLoop
(Graph* graph, Node* node, int level1, int level2)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoSwapLoop> InfoOp;
  UpdateNodeEdge( graph, node, InfoOp(graph,DepInfoSwapLoop(level1,level2)) );
}


template <class Graph, class Node>
void DepGraphNodeRemoveLoop (Graph* graph, Node* node, int level)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoRemoveLoop> InfoOp;
  UpdateNodeEdge( graph, node, InfoOp(graph,DepInfoRemoveLoop(level)) );
}

template <class Graph, class Node>
void DepGraphNodeMergeLoop (Graph* graph, Node* node,
                           int desc, int src)
{
  typedef UpdateDepEdgeInfo<Graph,DepInfoMergeLoop> InfoOp;
  UpdateNodeEdge ( graph, node, InfoOp(graph, DepInfoMergeLoop(desc,src)) );
}

template <class Graph, class Node>
void DepGraphNodeAlignLoop (Graph* graph, Node* node,
                           int level, int align)
{
  if (align != 0) {
    typedef UpdateDepEdgeInfo<Graph,DepInfoAlignLoop> InfoOp;
    UpdateNodeEdge ( graph, node, InfoOp(graph,DepInfoAlignLoop(level,-align)) );
  }
}


#endif
