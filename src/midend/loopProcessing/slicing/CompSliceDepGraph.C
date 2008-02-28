
#include <DepRel.h>
#include <LoopTreeDepComp.h>
#include <DepGraphTransform.h>
#include <DGBaseGraphImpl.h>
#include <CompSliceDepGraph.h>
#include <GraphGroup.h>

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
      std::cerr << "construct transitive dep. graph" << "\n";
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
 
    CompSliceDepGraphNode *src = treeMap.Map( s1 ), *snk = treeMap.Map(s2);
    if (src == snk)
      continue;
    CompSliceNest &nest1 = src->GetInfo(), &nest2 = snk->GetInfo();
    int dim1 = nest1.NumberOfEntries(), dim2 = nest2.NumberOfEntries();
    DepInfo dep = DepInfoGenerator::GetBottomDepInfo(dim1, dim2);
    for (int i = 0; i < dim1; i++) {
       CompSlice::SliceStmtInfo info1 = nest1[i]->QuerySliceStmtInfo(s1);
       int level1 = info1.loop->LoopLevel();
       for (int j = 0; j < dim2; j++) {
          CompSlice::SliceStmtInfo info2 = nest2[j]->QuerySliceStmtInfo(s2);
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
        vec.AttachObserver(*this);
        LoopTreeNode *r = c.GetLoopTreeRoot();
        if (r->ContainLoop())
           op.Analyze(c, vec);
        rootList.AppendLast(r);
      }
CompSliceDepGraphNode:: 
CompSliceDepGraphNode(MultiGraphCreate* g, LoopTreeDepComp &c, DependenceHoisting& op,
                        LoopTreeTransDepGraphCreate *t)
     : MultiGraphElem(g)
      { 
        vec.AttachObserver(*this);
        LoopTreeNode *r = c.GetLoopTreeRoot();
        if (r->ContainLoop())
           op.Analyze(c,t,vec);
        rootList.AppendLast(r);
      }

LoopTreeNodeIterator CompSliceDepGraphNode :: GetSliceRootIterator() const
{
  SinglyLinkedListWrap<LoopTreeNode*>::Iterator p(rootList);
  return new IteratorImplTemplate<LoopTreeNode*, SinglyLinkedListWrap<LoopTreeNode*>::Iterator>(p);
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
 CompSliceDepGraphNode* that = graph->QueryDepNode(&info.GetSliceNest2());
 rootList += that->rootList;
 FuseDepGraphNode(graph, this, that);
}

void CompSliceDepGraphNode::
UpdateDeleteEntry( const CompSliceNestDeleteEntryInfo &info)
{
  CompSliceDepGraphCreate* graph =
      static_cast<CompSliceDepGraphCreate*>(GetGraphCreate());
  DepGraphNodeRemoveLoop( graph, this, info.GetIndex());
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


