// tps (12/09/2009) : Playing with precompiled headers in Windows. Requires rose.h as the first line in source files.
#ifdef _MSC_VER
//#include "rose.h"
#endif

#include <LoopAnalysis.h>
#include <DepRel.h>
#include <DomainInfo.h>
#include <DepInfo.h>
#include <TransDepGraph.h>
#include <DepGraph.h>

template <class Node> 
LoopAlignInfo TransLoopFusible<Node>::
operator() ( TransDepGraphCreate<Node> *graph, 
             const LoopAnalInfo<Node> &l1, 
             const LoopAnalInfo<Node> &l2)
{
  int mina = 1, maxa = -1;

  DepInfoSetEdge* td12 = graph->GetTransDep( l1.n, l2.n);
  DepInfoSetEdge* td21 = graph->GetTransDep( l2.n, l1.n);

  if (td12 != 0) {
    DepRel r12 = td12->GetInfo().GetDepRel( l1.index, l2.index);
    DepDirType dir = r12.GetDirType();
    if (dir == DEPDIR_LE || dir == DEPDIR_EQ) 
      maxa = mina = td12->GetInfo().GetDepRel(l1.index, l2.index).GetMaxAlign();
  }
  if (mina <= maxa && td21 != 0) {
    DepRel r21 = td21->GetInfo().GetDepRel( l2.index,l1.index);
    DepDirType dir = r21.GetDirType();
    if (dir == DEPDIR_LE || dir == DEPDIR_EQ) 
      maxa = - td21->GetInfo().GetDepRel(l2.index, l1.index).GetMaxAlign();
    else
      maxa = mina-1;
  }
  return LoopAlignInfo(mina,maxa);
} 

template <class Node> 
bool TransLoopSlicable<Node>::
operator() ( TransDepGraphCreate<Node> *graph, const LoopAnalInfo<Node> &l)
{
  bool result = false;

  DepInfoSetEdge* td = graph->GetTransDep( l.n, l.n);
  if (td == 0)
    return true;

  DepRel r = td->GetInfo().GetDepRel( l.index, l.index );
  DepDirType dir = r.GetDirType();
  int a1 = r.GetMinAlign(), a2 = r.GetMaxAlign();
  if ( (dir == DEPDIR_LE && a2 <= 0) || (dir == DEPDIR_GE && a1 >= 0) 
       || (dir == DEPDIR_EQ && a2 <= 0))
      result = true;
  return result;
}

template <class Edge,class GraphCreate> 
bool PerfectLoopSlicable<Edge,GraphCreate>::
operator()(GraphCreate *g, int level)
{
  DepDirType cur = DEPDIR_EQ;
  for ( GraphEdgeIterator<GraphCreate> edgeIter(g);
       !edgeIter.ReachEnd(); ++edgeIter) {
     for (DepInfoConstIterator depIter = edgeIter.Current()->get_depIterator();
          !depIter.ReachEnd(); depIter++) { 
       DepInfo d = depIter.Current();
       DepRel r = d.Entry( level, level);
       DepDirType dir = r.GetDirType();
       int a1 = r.GetMinAlign(), a2 = r.GetMaxAlign();
       if (cur == DEPDIR_EQ)
           cur = dir;
       else if (dir != DEPDIR_EQ && cur != dir)
           return false;
       if ( (dir == DEPDIR_LE && a2 <= 0) || (dir == DEPDIR_GE && a1 >= 0)
             || (dir == DEPDIR_EQ && a2 <= 0))
          continue;
       return false;
     }
  }
  return true;
}

template <class Node> bool TransLoopReversible<Node>::
operator() ( TransDepGraphCreate<Node> *graph, const LoopAnalInfo<Node> &l)
{
  bool result = false;

  DepInfoSetEdge* td = graph->GetTransDep( l.n, l.n);
  if (td == 0)
    result = true;
  else {
    DepRel r = td->GetInfo().GetDepRel( l.index, l.index );
    DepDirType dir = r.GetDirType();
    int a1 = r.GetMinAlign();
    if ( dir == DEPDIR_EQ || ( dir == DEPDIR_GE && a1 >= 0) )
      result = true;
  }
  return result;
}

template <class Edge,class GraphCreate> 
bool PerfectLoopReversible<Edge,GraphCreate>::
operator() ( GraphCreate* g, int level)
{
  for ( GraphEdgeIterator<GraphCreate> edgeIter(g);
       !edgeIter.ReachEnd(); ++edgeIter) {
     for (DepInfoConstIterator depIter = edgeIter.Current()->get_depIterator();
          !depIter.ReachEnd(); depIter++) {
       DepInfo d = depIter.Current();
       DepRel r = d.Entry( level, level);
       DepDirType dir = r.GetDirType();
       int a1 = r.GetMinAlign(), a2 = r.GetMaxAlign();
       if ( (dir == DEPDIR_EQ && a1 == 0 && a2 == 0) || 
            ( dir == DEPDIR_GE && a1 >= 0) )
          continue;
       return false;
     }
  }
  return true;
}

template <class Node> bool TransLoopDistributable<Node>::
operator () ( TransDepGraphCreate<Node> *graph, const LoopAnalInfo<Node> &l1, 
                   const LoopAnalInfo<Node> &l2)
{
  bool result = false;;

  DepInfoSetEdge* td12 = graph->GetDepInfoSetEdge( l1.n, l2.n);
  DepInfoSetEdge* td21 = graph->GetDepInfoSetEdge( l2.n, l1.n);

  DepInfoSet  cycle12 = td12->GetInfo() * td21->GetInfo();
  DepInfoSet  cycle21 = td21->GetInfo() * td12->GetInfo();
  if (cycle12.IsTop() || cycle21.IsTop())
     result = true;
  else {
     DepRel r12 = td12->GetInfo().GetDepRel( l1.index, l2.index);
     DepRel r21 = td21->GetInfo().GetDepRel( l2.index, l1.index);
     if ( r12.GetDirType() == DEPDIR_EQ && r12.GetMinAlign() == 0 &&
          r12.GetMaxAlign() == 0)
         result = true;
     else if (r21.GetDirType() == DEPDIR_EQ && r21.GetMinAlign() == 0 &&
              r21.GetMaxAlign() == 0)
         result = true;
  }
  return result;
}

