
#include <stdlib.h>
#include <LoopTreeDepComp.h>
#include <LoopTreeBuild.h>

#include <DepGraphBuild.h>
#include <DepGraphTransform.h>
#include <DomainInfo.h>
#include <LoopTreeTransform.h>
#include <GraphIO.h>

void LoopTreeDepGraphCreate::AddNode(LoopTreeDepGraphNode* result)
    {
      LoopTreeNode *s = result->GetInfo();
      GraphCreateBase::AddNode(result);
      if (s && s->GetOrigStmt() != 0) {
        //s->AttachObserver(*this);
        if (map.GetDepNode(s) == 0)
           map.InsertMapping(s, result);
        else
           assert(map.GetDepNode(s) == result);
      }
    }

LoopTreeDepGraphNode* LoopTreeDepGraphCreate::
CreateNode(LoopTreeNode *s, const DomainCond& c)
   {  
      LoopTreeDepGraphNode *result = map.GetDepNode(s);
      if (result == 0)  {
         result = new LoopTreeDepGraphNode(this, s, c);
         AddNode(result);
      }
      return result;
   }

LoopTreeDepGraphNode* LoopTreeDepGraphCreate::
CreateNode(LoopTreeNode *s, const DepInfo& m, const DomainCond& c)
   {  
      LoopTreeDepGraphNode *result = map.GetDepNode(s);
      if (result == 0) {
         result = new LoopTreeDepGraphNode(this, s, m, c);
         AddNode(result);
      }
      return result;
   }

LoopTreeDepGraphNode* LoopTreeDepGraphCreate::
CreateNode(LoopTreeNode *s, LoopTreeDepGraphNode* that )
    {
      LoopTreeDepGraphNode *result = map.GetDepNode(s);
      if (result == 0) {
         result = (that == 0)? new LoopTreeDepGraphNode(this, s) :
                  new LoopTreeDepGraphNode(this, s, that->loopMap, that->domain);
         AddNode(result);
      }
      return result;
    }

bool LoopTreeDepGraphCreate::DeleteNode( LoopTreeDepGraphNode *n)
    {
      LoopTreeNode *s = n->GetInfo();
      if (s && s->GetOrigStmt() != 0) {
           s->DetachObserver(*this);
      }
      return DepInfoGraphCreate<LoopTreeDepGraphNode>::DeleteNode(n);
    }

DepInfoEdge* LoopTreeDepGraphCreate::
CreateEdge( LoopTreeDepGraphNode *n1, LoopTreeDepGraphNode *n2, const DepInfo& info)
   { 
     //assert(info.GetDepType() != DEPTYPE_NONE);
     return DepInfoGraphCreate<LoopTreeDepGraphNode>::CreateEdge(n1,n2,info);
   }

DepInfoEdge* LoopTreeDepGraphCreate::
CreateEdgeFromOrigAst( LoopTreeDepGraphNode *n1, LoopTreeDepGraphNode *n2,
                                      const DepInfo& info)
    {
      DepInfo info1 =
        (n1->GetInfo()->GetOrigStmt() != 0 && !n1->GetLoopMap().IsID())?
        ::Reverse(n1->GetLoopMap()) * info : info;
      if (n2->GetInfo()->GetOrigStmt() != 0 && !n2->GetLoopMap().IsID())
        info1 = info1 * n2->GetLoopMap();
      return CreateEdge(n1,n2,info1);
    }


void LoopTreeDepComp :: DumpTree() const 
  { GetLoopTreeRoot()->DumpTree(); }
void LoopTreeDepComp :: DumpDep() const 
{ 
   GraphAccessTemplate<LoopTreeDepGraphNode, DepInfoEdge>::NodeIterator nodes
             = GetDepGraph()->GetNodeIterator();
   assert(!nodes.ReachEnd());
   write_graph(*GetDepGraph(), std::cerr, std::string("dep"));
}
void LoopTreeDepComp :: DumpNode( LoopTreeNode *s) const
   { std::cerr << GraphNodeToString(*GetDepGraph(),  GetDepNode(s) ) << std::endl; }

void LoopTreeDepGraphCreate :: UpdateSplitStmt2( const SplitStmtInfo2 &info)
{
  assert(false);
}

void LoopTreeDepGraphCreate :: UpdateSplitStmt( const SplitStmtInfo &info)
{
  LoopTreeNode* orig = info.GetObserveNode(), *split = info.GetSplitStmt();
  LoopTreeDepGraphNode *n = map.GetDepNode(orig);
  DomainCond cond1(n->domain), cond2(n->domain);

  LoopTreeDepGraphNode *n1 = map.GetDepNode(split);
  if (n1 == 0) {
     cond1.SetLoopRel( info.GetLoop1(), info.GetLoop2(), info.GetRel());
     cond2.SetLoopRel( info.GetLoop1(), info.GetLoop2(), - info.GetRel());
     n1 = CreateNode( split, n->loopMap, cond1);
     split->AttachObserver(*this);
     n->domain = cond2;
  }
  else {
    AddNode(n1);
    cond1 = n1->domain;
  }
  CopySplitNodeEdge( this,n, n1, cond1);
  DepGraphNodeRestrictDomain (this,n,cond2); 
}

LoopTreeDepGraphCreate::  ~LoopTreeDepGraphCreate()
{
}

void LoopTreeDepGraphCreate :: UpdateDeleteNode( const LoopTreeNode* s)
{ 
  LoopTreeDepGraphNode *n = map.GetDepNode(s);
  if (n != 0) {
    n->GetInfo() = 0;
    DeleteNode(n);
  }
}

void LoopTreeDepGraphCreate :: 
UpdateDistNode( const DistNodeInfo &info)
{ 
 LoopTreeNode* orig = info.GetObserveNode(), *n = info.GetNewNode();
 int level = orig->LoopLevel();
 LoopTreeTraverseSelectStmt p1(orig);
 for ( ; !p1.ReachEnd(); p1.Advance()) {
    LoopTreeDepGraphNode* stmt1 = map.GetDepNode(p1.Current());
    for (LoopTreeTraverseSelectStmt p2(n); !p2.ReachEnd(); p2.Advance()) {
       LoopTreeDepGraphNode* stmt2 = map.GetDepNode(p2.Current());
       GraphCrossEdgeIterator<LoopTreeDepGraphCreate> edges(this,stmt1,stmt2);
       for ( ; !edges.ReachEnd(); ++edges) {
          DepInfoEdge* e = (*edges);
          e->GetInfo().DistLoop(level);
       }
       edges=GraphCrossEdgeIterator<LoopTreeDepGraphCreate>(this,stmt2,stmt1);
       for ( ; !edges.ReachEnd(); ++edges) {
          DepInfoEdge* e = (*edges);
          e->GetInfo().DistLoop(level);
       }
    }
 } 
}

void LoopTreeDepGraphCreate :: 
UpdateInsertLoop( const InsertLoopInfo &info)
{ 
  PtrSetWrap<LoopTreeDepGraphNode> nodes;
  LoopTreeNode* l = info.GetObserveNode();
  LoopTreeTraverseSelectStmt iter(l);
  for (LoopTreeNode *s; (s = iter.Current()); iter.Advance()) {
     LoopTreeDepGraphNode* n = map.GetDepNode(s);
     nodes.insert(n);
  }
  SelectPtrSet<LoopTreeDepGraphNode> selnode(nodes);
  DepGraphInsertLoop( this, selnode, l->LoopLevel()); 
}

void LoopTreeDepGraphCreate :: UpdateSwapStmtLoop( const SwapStmtLoopInfo &info)
{
 DepGraphNodeSwapLoop( this, map.GetDepNode(info.GetObserveNode()), 
                        info.GetLoop1(), info.GetLoop2());
}

void LoopTreeDepGraphCreate :: 
UpdateDeleteStmtLoop( const DeleteStmtLoopInfo &info)
{
 DepGraphNodeRemoveLoop( this, map.GetDepNode(info.GetObserveNode()), info.GetLoop());
}

void LoopTreeDepGraphCreate :: 
UpdateMergeStmtLoop( const MergeStmtLoopInfo &info)
{
  LoopTreeDepGraphNode *node = map.GetDepNode(info.GetObserveNode());
  int desc = info.GetLoop1();
  DepGraphNodeMergeLoop( this, node, desc, info.GetLoop2());
  DepGraphNodeAlignLoop( this, node, desc, info.GetMergeAlign());
}

class BuildLoopDepGraphEdges : public AstTreeDepGraphBuildImpl
{
  virtual GraphAccessInterface::Node* 
  CreateNodeImpl(AstNodePtr start, const DomainCond& c) 
  { assert(false); return 0; } 
  virtual void
          CreateEdgeImpl(GraphAccessInterface::Node *gn1, GraphAccessInterface::Node *gn2, DepInfo info) 
        { LoopTreeDepGraphNode *n1 = static_cast<LoopTreeDepGraphNode*>(gn1),
                               *n2 = static_cast<LoopTreeDepGraphNode*>(gn2);
          if (info.GetDepType() == DEPTYPE_TRANS) {
             GraphCrossEdgeIterator<LoopTreeDepGraphCreate> 
                     crossIter(&graph,n1, n2);
             for ( ; !crossIter.ReachEnd(); ++crossIter) {
               DepInfoEdge *e = crossIter.Current();
               if (e->GetInfo() == info)
                  return;
             }
          }
          graph.CreateEdgeFromOrigAst(n1,n2,info);
        }
  virtual DepInfoConstIterator
          GetDepInfoIteratorImpl( GraphAccessInterface::Edge* ge, DepType t) 
        { DepInfoEdge *e = static_cast<DepInfoEdge*>(ge);
          return SelectDepType(e->GetInfo(),t)? 
	        DepInfoConstIterator(new SingleIterator<DepInfo>(e->GetInfo())) 
	         : DepInfoConstIterator(); 
        }
  virtual AstNodePtr GetNodeAst( GraphAccessInterface::Node *gn) 
        { 
            LoopTreeDepGraphNode *n = static_cast<LoopTreeDepGraphNode*>(gn);
            return n->GetInfo()->GetOrigStmt();
        }
  virtual const GraphAccessInterface* Access() const { return &ga; }
 protected:
  LoopTreeDepGraphCreate &graph;
  GraphAccessWrapTemplate<GraphAccessInterface::Node,
                          GraphAccessInterface::Edge,
                          LoopTreeDepGraphCreate> ga;
 public:
  BuildLoopDepGraphEdges( LoopTreeDepGraphCreate &c) : graph(c),ga(&c) {}
};

void LoopTreeDepGraphCreate :: 
BuildDep( LoopTransformInterface &fa, DepInfoAnal &anal, LoopTreeDepGraphNode *n1,
          LoopTreeDepGraphNode *n2, DepType t)
{
 BuildLoopDepGraphEdges b(*this);
 AstTreeDepGraphAnal build(&b,anal);
 typedef AstTreeDepGraphAnal::StmtNodeInfo StmtNodeInfo;
 StmtNodeInfo analInfo1(n1, n1->GetInfo()->GetOrigStmt());
 StmtNodeInfo analInfo2(n2, n2->GetInfo()->GetOrigStmt());
 build.ComputeDataDep(fa, analInfo1, analInfo2, t);
}

class BuildLoopDepGraphCreate : public BuildLoopDepGraphEdges
{
  virtual GraphAccessInterface::Node* CreateNodeImpl(AstNodePtr start, const DomainCond& c)
  {
    LoopTreeNode *cur = iter.Current();
    for ( ; (!cur->IncreaseLoopLevel() && cur->GetOrigStmt()==0);
         iter.Advance(), cur = iter.Current());
    assert( cur->GetOrigStmt() == 0 || cur->GetOrigStmt() == start);
    iter.Advance();
    LoopTreeDepGraphNode *d = graph.CreateNode(cur, c);
    return d;
  }
  LoopTreeTraverse iter;
 public:
  BuildLoopDepGraphCreate( LoopTreeNode* root, LoopTreeDepGraphCreate &c)
    : BuildLoopDepGraphEdges(c), iter(root, LoopTreeTraverse::PreOrder) {}
};

void LoopTreeDepCompCreate :: BuildDepGraph( LoopTransformInterface &la)
{
  assert( depCreate == 0);
  depCreate = new LoopTreeDepGraphCreate(&nodeMap);
  treeCreate.AttachObserver(*depCreate);
  BuildLoopDepGraphCreate depImpl(treeCreate.GetTreeRoot(), *depCreate);
  BuildAstTreeDepGraph proc ( la, &depImpl, anal);
  bool succ = ReadAstTraverse(la, top, proc, AstInterface::PreAndPostOrder);
  assert(succ);

  proc.TranslateCtrlDeps(la);
  PtrSetWrap <LoopTreeDepGraphNode> nodeSet;
  LoopTreeDepGraphCreate::NodeIterator iter= depCreate->GetNodeIterator();
  for ( ; !iter.ReachEnd(); ++iter) {
      LoopTreeDepGraphNode *n = iter.Current();
      if (n->GetInfo()->IncreaseLoopLevel()) {
          nodeSet.insert(n);
      }
  }
  for (PtrSetWrap<LoopTreeDepGraphNode>::const_iterator ctrlIter=nodeSet.begin();
       !ctrlIter.ReachEnd(); ++ctrlIter) {
       LoopTreeDepGraphNode *n = ctrlIter.Current();
       depCreate->DeleteNode(n);
  }
}

void LoopTreeDepCompCreate:: DetachDepGraph()
{
  if (depCreate != 0) {
     treeCreate.DetachObserver(*depCreate);
     delete depCreate;
     depCreate = 0;
     SetDepGraph( depCreate);
  }
}

LoopTreeDepCompCreate :: ~LoopTreeDepCompCreate()
{
   DetachDepGraph();
}

LoopTreeDepCompCreate :: 
LoopTreeDepCompCreate( LoopTransformInterface &la, const AstNodePtr& _top,
                       bool builddep)
  : anal(la), depCreate(0), top(_top)
{
  AstInterface& fa = la;
  assert(fa.GetRoot() != 0);
  SetTreeRoot(treeCreate.GetTreeRoot());
  SetMap(&nodeMap);

  LoopTreeBuild treeproc;
  bool succ = treeproc(la, top, &treeCreate, &la);
  SetTreeCreate(&treeCreate);

  assert(succ); 
  if (builddep) 
     BuildDepGraph(la);
  SetDepGraph( depCreate);
}

AstNodePtr LoopTreeDepCompCreate :: 
CodeGen( LoopTransformInterface &fa )
{ 
  AstNodePtr result = treeCreate.CodeGen(fa); 
  return result;
}

void LoopTreeDepCompCreate :: UpdateDeleteNode( const MultiGraphElem *n)
{
   const MultiGraphObserveNodeTemplate<LoopTreeNode*> *nn 
      = static_cast < const MultiGraphObserveNodeTemplate<LoopTreeNode*> * >(n);
   LoopTreeNode *s = nn->GetInfo();
   nodeMap.RemoveMapping(s);
}

LoopTreeDepCompSubtree :: 
LoopTreeDepCompSubtree( LoopTreeDepComp &comp, LoopTreeNode *t)
  : LoopTreeDepComp(comp) 
{
 SetTreeRoot(t);
 SetDepGraph(new LoopTreeDepGraphSubtree(comp, t, comp.GetDepGraph()));
}

#ifndef NO_TEMPLATE_INSTANTIATION
#define TEMPLATE_ONLY

#include "TransDepGraphImpl.h"
template class DepInfoSetGraphCreate<LoopTreeDepGraphNode>;
template class TransDepGraphCreate<LoopTreeDepGraphNode>;
template class SelectPtrSet<const LoopTreeDepGraphNode>;
#include <LoopAnalysis.C>
template class PerfectLoopSlicable<DepInfoEdge, LoopTreeDepGraph>;
template class PerfectLoopReversible<DepInfoEdge, LoopTreeDepGraph>;
template class TransLoopSlicable<LoopTreeDepGraphNode>;
template class TransLoopReversible<LoopTreeDepGraphNode>;
template class TransLoopFusible<LoopTreeDepGraphNode>;
#endif
