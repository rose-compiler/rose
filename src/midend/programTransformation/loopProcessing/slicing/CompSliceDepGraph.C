
#include <DepRel.h>
#include <GraphIO.h>
#include <LoopTreeDepComp.h>
#include <LoopTreeTransform.h>
#include <DepGraphTransform.h>
#include <DGBaseGraphImpl.h>
#include <CompSliceDepGraph.h>
#include <GraphGroup.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

extern bool DebugDep();

CompSliceDepGraphCreate ::
CompSliceDepGraphCreate( LoopTreeDepComp &comp, DependenceHoisting& op,
                         LoopTreeTransDepGraphCreate *tc)
 : DepInfoGraphCreate<CompSliceDepGraphNode>(impl = new DAGBaseGraphImpl())
{
  LoopTreeNode *root = comp.GetLoopTreeRoot();
  PtrMapWrap <LoopTreeNode, CompSliceDepGraphNode> treeMap;
  for (LoopTreeNode *n = root->FirstChild(); n != 0; n = n->NextSibling()) {
    LoopTreeDepCompSubtree scope(comp, n);
    CompSliceDepGraphNode* sliceNode = 0;
    if (tc == 0 || !n->ContainLoop() || n->IsPerfectLoopNest()) {
       sliceNode = CreateNode(scope, op);
    }
    else {
      if (DebugDep())
         std::cerr << "construct transitive dep. graph for " << n->TreeToString() << "\n";
      sliceNode = CreateNode(scope, op, tc);
    }
    LoopTreeTraverseSelectStmt stmts(n);
    for (LoopTreeNode *s; (s = stmts.Current()); stmts.Advance())
        treeMap.InsertMapping(s, sliceNode);
  }
  LoopTreeDepGraph* depGraph = comp.GetDepGraph();
  for (GraphEdgeIterator<LoopTreeDepGraph> depIter(depGraph);
       !depIter.ReachEnd(); ++depIter) {
    DepInfoEdge* edge = depIter.Current();
    LoopTreeDepGraphNode *n1 = depGraph->GetEdgeEndPoint(edge, EdgeOut);
    LoopTreeDepGraphNode *n2 = depGraph->GetEdgeEndPoint(edge, EdgeIn);

    LoopTreeNode *s1 = comp.GetTreeNode(n1);
    LoopTreeNode *s2 = comp.GetTreeNode(n2);
    assert(s1 != 0 && s2 != 0);
    CompSliceDepGraphNode *src = treeMap.Map( s1 ), *snk = treeMap.Map(s2);
    if (src == 0 || snk == 0 || src == snk) {
      continue;
   }
    CompSliceNest *nest1 = src->GetInfo().GetNest(), *nest2 = snk->GetInfo().GetNest();
    int dim1 = (nest1==0)? 0 : nest1->NumberOfEntries();
    int dim2 = (nest2==0)? 0 : nest2->NumberOfEntries();
    DepInfo dep = DepInfoGenerator::GetBottomDepInfo(dim1, dim2);
    for (int i = 0; i < dim1; i++) {
        CompSlice::SliceStmtInfo info1=nest1->Entry(i)->QuerySliceStmtInfo(s1);
        int level1 = info1.loop->LoopLevel();
        for (int j = 0; j < dim2; j++) {
           CompSlice::SliceStmtInfo info2=nest2->Entry(j)->QuerySliceStmtInfo(s2);
           int level2 = info2.loop->LoopLevel();
           DepRel rel = edge->GetInfo().Entry( level1, level2);
           rel.IncreaseAlign(info1.align - info2.align);
           dep.Entry(i,j) = rel;
        }
    }
    CreateEdge(src, snk, dep);
   }
}

void CompSliceDepGraphCreate :: TopoSort( bool reverse)
{
  impl->TopoSort(reverse);
}

CompSliceDepGraphNode ::
CompSliceDepGraphNode(MultiGraphCreate* g, LoopTreeDepComp &c, DependenceHoisting &op)
     : MultiGraphElem(g)
      {
        LoopTreeNode *r = c.GetLoopTreeRoot();
        if (r->ContainLoop()) {
           FullNestInfo* info = new FullNestInfo();
           op.Analyze(c, info->vec);
           if (info->vec.NumberOfEntries() > 0) {
              info->vec.AttachObserver(*this);
              impl = info;
              return;
           }
           delete info;
        }
        EmptyNestInfo* info = new EmptyNestInfo(r);
        impl = info;
      }
CompSliceDepGraphNode::
CompSliceDepGraphNode(MultiGraphCreate* g, LoopTreeDepComp &c, DependenceHoisting& op,
                        LoopTreeTransDepGraphCreate *t)
     : MultiGraphElem(g)
      {
        LoopTreeNode *r = c.GetLoopTreeRoot();
        if (r->ContainLoop()) {
           FullNestInfo* info = new FullNestInfo();
           op.Analyze(c,t, info->vec);
           if (info->vec.NumberOfEntries() > 0) {
              info->vec.AttachObserver(*this);
              impl = info;
              return;
           }
           delete info;
        }
        EmptyNestInfo* info = new EmptyNestInfo(r);
        impl = info;
      }

void CompSliceDepGraphNode:: UpdateSwap(const CompSliceNestSwapInfo &info)
{
 CompSliceDepGraphCreate* graph =
    static_cast<CompSliceDepGraphCreate*>(GetGraphCreate());
 DepGraphNodeSwapLoop( graph, this, info.GetIndex1(),info.GetIndex2());
}

void CompSliceDepGraphNode:: UpdateFusion(const CompSliceNestFusionInfo& info)
{
 CompSliceDepGraphCreate* graph =
    static_cast<CompSliceDepGraphCreate*>(GetGraphCreate());
 const CompSliceNest& g1 = info.GetSliceVec();
 const CompSliceNest& g2 = info.GetSliceNest2();
 CompSliceDepGraphNode* that = graph->QueryDepNode(&g2);

    if (that->impl->innerNest != 0 || impl->innerNest != 0) {
       NestInfo* p = impl->innerNest, *q = that->impl->innerNest;
       if (p == 0) {
          EmptyNestInfo* info = new EmptyNestInfo();
          p = impl->innerNest = info;
          for (CompSlice::ConstStmtIterator stmts = g1[0]->GetConstStmtIterator(); !stmts.ReachEnd(); ++stmts) {
              LoopTreeNode* cur = stmts.Current();
              if (!g2[0]->QuerySliceStmt(cur))
                info->nodes.insert(cur);
           }
       }
       if (q == 0)
          q = new EmptyNestInfo(that->impl->GetNest()->Entry(0));
       while (p->innerNest != 0) p = p->innerNest;
       p->innerNest = q;
    }

 FuseDepGraphNode(graph, this, that);
}

void CompSliceDepGraphNode::
UpdateDeleteEntry( const CompSliceNestDeleteEntryInfo &info)
{
  int index = info.GetIndex();
  CompSliceDepGraphCreate* graph =
      static_cast<CompSliceDepGraphCreate*>(GetGraphCreate());
  DepGraphNodeRemoveLoop( graph, this, index);

  if (!info.SaveAsInner()) return;

  const CompSliceNest& sliceVec = info.GetSliceVec();
  int size = sliceVec.NumberOfEntries();
  if (impl->innerNest == 0) {
       impl->innerNest = new FullNestInfo(const_cast<CompSlice*>(sliceVec[index]),size);
  }
  else {
      FullNestInfo* fullImpl = dynamic_cast<FullNestInfo*>(impl->innerNest);
      assert(fullImpl!=0 && fullImpl->vec[0]->SliceCommonStmt(sliceVec[index]));
      fullImpl->vec.Append(const_cast<CompSlice*>(sliceVec[index]));
   }
}

void CompSliceDepGraphNode::
UpdateDuplicateEntry( const CompSliceNestDuplicateEntryInfo &info)
{
  int desc =info.GetDescIndex(), src = info.GetSrcIndex();
  CompSliceDepGraphCreate* graph =
      static_cast<CompSliceDepGraphCreate*>(GetGraphCreate());
  DepGraphNodeInsertLoop( graph, this, desc);
  if (src > desc)
    ++src ;
  DepGraphNodeMergeLoop( graph, this, desc, src);
}

void CompSliceDepGraphNode::
UpdateAlignEntry( const CompSliceNestAlignEntryInfo &info)
{
  CompSliceDepGraphCreate* graph =
      static_cast<CompSliceDepGraphCreate*>(GetGraphCreate());
  DepGraphNodeAlignLoop( graph, this, info.GetIndex(), info.GetAlign());
}

void CompSliceDepGraphCreate:: MoveEdgeEndPoint(DepInfoEdge *e, EdgeDirection dir,
                        CompSliceDepGraphNode *n)
     {
       // CompSliceDepGraphNode *n1 = GetEdgeEndPoint(e, dir);
       CompSliceDepGraphNode *n2 = GetEdgeEndPoint(e, GraphAccess::Reverse(dir));
       if (n == n2) {
          DeleteEdge(e);
       }
       else {
         GraphCrossEdgeIterator<CompSliceDepGraph> outIter(this, n, n2);
         GraphCrossEdgeIterator<CompSliceDepGraph> inIter(this, n2, n);
         DepInfoEdge *e1 = 0;
         if (  dir==GraphAccess::EdgeOut && !outIter.ReachEnd())
             e = outIter.Current();
         else if (dir==GraphAccess::EdgeIn && !inIter.ReachEnd())
             e = inIter.Current();
         if (e1 == 0) {
            DepInfoGraphCreate<CompSliceDepGraphNode>::MoveEdgeEndPoint(e,dir,n);
         }
         else {
            e1->GetInfo() |= e->GetInfo();
            DeleteEdge(e);
         }
       }
     }

DepInfoEdge* CompSliceDepGraphCreate::
CreateEdge( CompSliceDepGraphNode *n1, CompSliceDepGraphNode *n2, const DepInfo &info)
       {
        GraphCrossEdgeIterator<CompSliceDepGraph> crossIter(this,n1,n2);
        DepInfoEdge *e = 0;
         if (crossIter.ReachEnd()) {
           e = new DepInfoEdge(this, info);
           AddEdge(n1,n2, e);
         }
         else {
            e = crossIter.Current();
            e->GetInfo() |= info;
         }
         return e;
       }


LoopTreeNode* CompSliceDepGraphNode::FullNestInfo::
GenXformRoot(LoopTreeNode* top) const
{
   if (vec.NumberOfEntries() == 0)
    {
      std::cerr << "error: empty slice : " << vec.toString() << "\n";
      assert(0);
    }
   const CompSlice* cur = vec[0];
   CompSliceSelect sel(cur);
   LoopTreeNode* res = LoopTreeDistributeNode()(top, sel,LoopTreeDistributeNode::BEFORE);
   assert(res != 0);
   return res;
}

LoopTreeNode* CompSliceDepGraphNode::EmptyNestInfo::
GenXformRoot(LoopTreeNode* top) const
{
  SelectSTLSet<LoopTreeNode*> sel(nodes);
  LoopTreeNode* res = LoopTreeDistributeNode()(top, sel,LoopTreeDistributeNode::BEFORE);
  return res;
}
