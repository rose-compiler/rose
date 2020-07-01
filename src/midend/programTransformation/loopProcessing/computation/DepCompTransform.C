#include <stdlib.h>
#include <sstream>

#include <CommandOptions.h>
#include <LoopInfoInterface.h>
#include <DepCompTransform.h>
#include <LoopTreeTransform.h>
#include <SCCAnalysis.h>
#include <TypedFusion.h>
#include <union_find.h>
#include <GraphScope.h>
#include <GraphIO.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

bool DebugRefFuse()
{
  static int r = 0;
  if (r == 0) {
     r = CmdOptions::GetInstance()->HasOption("-debugreffuse")? 1 : -1;
  }
  return r == 1;
}
bool DebugCopyConfig()
{
  static int r = 0;
  if (r == 0) {
     r = CmdOptions::GetInstance()->HasOption("-debugcopyconfig")? 1 : -1;
  }
  return r == 1;
}
bool DebugCopySplit()
{
  static int r = 0;
  if (r == 0) {
     r = CmdOptions::GetInstance()->HasOption("-debugcopysplit")? 1 : -1;
  }
  return r == 1;
}
bool DebugLoopDist()
{
  static int r = 0;
  if (r == 0) {
     r = CmdOptions::GetInstance()->HasOption("-debugloopdist")? 1 : -1;
  }
  return r == 1;
}
class CollectTreeNodes {
   LoopTreeDepComp& comp;
   PtrSetWrap<LoopTreeNode>& result;
 public:
   CollectTreeNodes(LoopTreeDepComp& c, PtrSetWrap<LoopTreeNode>& r)
       : comp(c),result(r) {}
   bool operator()(const LoopTreeDepGraphNode* n)
    {
      LoopTreeNode* s = comp.GetTreeNode(n);
      if (result.IsMember(s))
         return false;
      result.insert(s);
      return true;
    }
};
void DepLinkedNodes(LoopTreeDepComp& comp, LoopTreeNodeIterator stmts,
                    GraphAccess::EdgeDirection dir, PtrSetWrap<LoopTreeNode>& result)
{
  CollectTreeNodes collect(comp, result);

  LoopTreeDepGraph* depGraph =  comp.GetDepGraph();
  for ( ; !stmts.ReachEnd(); stmts++) {
    LoopTreeNode *s = stmts.Current();
    if (result.IsMember(s))
      continue;
    LoopTreeDepGraphNode *n = comp.GetDepNode(s);
    GraphGetNodeReachable<LoopTreeDepGraph,CollectTreeNodes>()(depGraph, n, dir, collect);
  }
}


DepCompDistributeLoop::Result DepCompDistributeLoop ::
operator()( LoopTreeDepComp& tc, LoopTreeNode *l, LoopTreeNodeIterator stmts)
{
  typedef PtrSetWrap<LoopTreeNode> LoopTreeNodeList;
  LoopTreeNodeList stmts1, stmts2;

  LoopTreeDepCompSubtree loopComp( tc, l);
if (DebugLoopDist()) {
std::cerr << "***************Loop Distribution: From sub tree *******\n";
loopComp.DumpTree();
loopComp.DumpDep();
std::cerr << "***************end sub tree *******\n";
}
  DepLinkedNodes( loopComp, stmts, GraphAccess::EdgeIn, stmts1);
  DepLinkedNodes( loopComp, stmts, GraphAccess::EdgeOut, stmts2);

  stmts2 &= stmts1;
  stmts1 -= stmts2;

  if (stmts1.NumberOfEntries()) {
    SelectPtrSet<LoopTreeNode> sel(stmts1);
    LoopTreeDistributeNode()(l,sel);
  }
  SelectPtrSet<LoopTreeNode> sel2(stmts2);
  return Result( LoopTreeDistributeNode()(l, sel2), stmts2);
}

DepCompDistributeLoop::Result DepCompDistributeLoop ::
operator() ( LoopTreeDepComp &tc, LoopTreeNode *l)
{
  typedef PtrSetWrap<LoopTreeNode> LoopTreeNodeSet;

  LoopTreeDepCompSubtree loopComp( tc, l);
if (DebugLoopDist()) {
std::cerr << "**Loop distribution based on SCC analysis From sub tree *******\n";
loopComp.DumpTree();
loopComp.DumpDep();
std::cerr << "***************end sub tree *******\n";
std::cerr << "***************original dependence graph *******\n";
tc.DumpDep();
}
  Result result;

  LoopTreeDepGraph *depGraph = loopComp.GetDepGraph();
  GraphAccessWrapTemplate<void,void,LoopTreeDepGraph> access(depGraph);
  SCCGraphCreate sccGraph( &access );
  sccGraph.TopoSort();
  //write_graph(sccGraph, std::cerr, std::string("scc dump"));

  for ( GroupGraphCreate::NodeIterator sccIter = sccGraph.GetNodeIterator();
       !sccIter.ReachEnd(); sccIter++) {
    GroupGraphNode *scc = sccIter.Current();
    LoopTreeNodeSet treeSet;
    for (GroupGraphNode::const_iterator iter=scc->begin(); !iter.ReachEnd(); iter++) {
       LoopTreeDepGraphNode *n =
          static_cast<LoopTreeDepGraphNode*>(iter.Current());
       treeSet.insert( tc.GetTreeNode(n) );
    }
    if (treeSet.NumberOfEntries()) {
      SelectPtrSet<LoopTreeNode> sel(treeSet);
      LoopTreeNode* tmp = LoopTreeDistributeNode()( l,sel);
      if (result.node == 0) {
         result.node = tmp; result.sel = treeSet;
      }
    }
  }
if (DebugLoopDist()) {
std::cerr << "**After loop distribution *******\n";
loopComp.DumpTree();
loopComp.DumpDep();
std::cerr << "***************end sub tree *******\n";
}
  return result;
}

class DepCompAstRefDAG : public DepCompAstRefGraphCreate
{
public:
  DepCompAstRefDAG(const DepCompAstRefAnal& stmtorder, const DepCompAstRefGraphCreate* g)
   {
      DoublyLinkedListWrap <DepCompAstRefGraphNode*> nodelist;
      DepCompAstRefGraphCreate::NodeIterator nodes = g->GetNodeIterator();
      for ( ; !nodes.ReachEnd(); nodes.Advance()) {
           DepCompAstRefGraphNode* n = nodes.Current();
           AddNode(n);
           nodelist.AppendLast(n);
      }
      if (nodelist.size() <= 1)
          return;
      for (nodes.Reset(); !nodes.ReachEnd(); nodes.Advance()) {
         DepCompAstRefGraphNode* n = *nodes;
         DepCompAstRef& info = n->GetInfo();
         for (DepInfoEdgeIterator edges = g->GetNodeEdgeIterator(n, GraphAccess::EdgeOut);
               !edges.ReachEnd(); ++edges) {
             DepInfoEdge* e = *edges;
             DepCompAstRefGraphNode* n1 = g->GetEdgeEndPoint(e, GraphAccess::EdgeIn);
             DepCompAstRef& info1 = n1->GetInfo();
             int c = stmtorder.CompareAstRef(info,info1);
             if (c < 0) {
                   AddEdge(n, n1, e);
             }
             else if (c > 0) {
                  AddEdge(n1,n,e);
             }
         }
      }
      if (DebugRefFuse()) {
         std::cerr << GraphToString(*this) << std::endl;
      }
   }
};

class AstRefTypedFusionOperator : public TypedFusionOperator
{
    struct FuseNodeInfo {
        int nodetype;
        DepCompCopyArrayCollect::CopyArrayUnit* collect;
        FuseNodeInfo(int t = -1, DepCompCopyArrayCollect::CopyArrayUnit* c = 0)
           : nodetype(t), collect(c) {}
    };
    std::map<DepCompAstRefGraphNode*,FuseNodeInfo, std::less<DepCompAstRefGraphNode*> > nodeMap;
    int size;
    DepCompCopyArrayCollect& collect;

    void AddNodeType( const DepCompAstRefGraphCreate *g, DepCompAstRefGraphNode* n)
     {
       for (GraphAccess::EdgeDirection d = GraphAccess::EdgeOut; d != GraphAccess::BiEdge;
            d = (d == GraphAccess::EdgeOut)? GraphAccess::EdgeIn : GraphAccess::BiEdge) {
         DepCompAstRefGraphCreate::EdgeIterator edges = g->GetNodeEdgeIterator(n, d);
         for ( ; !edges.ReachEnd(); ++edges) {
            DepInfoEdge* e = (*edges);
            DepCompAstRefGraphNode* n1 = g->GetEdgeEndPoint(e, GraphAccess::Reverse(d));
            if  (nodeMap.find(n1) != nodeMap.end())
                  continue;
            if (DebugRefFuse())
              std::cerr << "mapping node " << n1->toString() << " to " << size << std::endl;
            nodeMap[n1] = FuseNodeInfo(size);
            AddNodeType(g, n1);
         }
       }
     }
  public:
    AstRefTypedFusionOperator(DepCompCopyArrayCollect& c,
                              const DepCompAstRefGraphCreate* g)
         : collect(c)
      {
        size = 0;
        AstInterface& fa = LoopTransformInterface::getAstInterface();
        for (DepCompAstRefGraphCreate::NodeIterator nodes = g->GetNodeIterator();
            !nodes.ReachEnd(); nodes.Advance()) {
           DepCompAstRefGraphNode* n = nodes.Current();
           if (nodeMap.find(n) != nodeMap.end())
              continue;
           AstNodePtr r = n->GetInfo().orig;
           AstNodePtr arr;
           if (LoopTransformInterface::IsArrayAccess(r, &arr) && fa.IsVarRef(arr))  {
              if (DebugRefFuse())
                  std::cerr << "mapping node " << n->toString() << " to " << size << std::endl;
              nodeMap[n] = size;
              AddNodeType(g, n);
              ++size;
           }
       }
      }
    void operator()(DepCompAstRefDAG* dag)
      {
        GraphAccessWrapTemplate<void,void,DepCompAstRefDAG> access(dag);
        for (int i = 0; i < size; ++i) {
           if (DebugRefFuse())
              std::cerr << "fusing node type " << i << std::endl;
           TypedFusion()(&access, *this, i);
        }
        for (DepCompAstRefGraphCreate::NodeIterator nodes = dag->GetNodeIterator();
            !nodes.ReachEnd(); nodes.Advance()) {
           DepCompAstRefGraphNode* n1 = *nodes;
           FuseNodeInfo &info1 = nodeMap[n1];
           if (info1.collect == 0) {
              info1.collect = &collect.AddCopyArray();
              info1.collect->refs.insert(n1);
           }
        }
      }
    virtual int GetNodeType( GraphAccessInterface::Node *n)
      {
         DepCompAstRefGraphNode* n1 = static_cast<DepCompAstRefGraphNode*>(n);
         if (nodeMap.find(n1) == nodeMap.end()) {
            return nodeMap.size();
         }
         else {
            return nodeMap[n1].nodetype;
         }
      }
    virtual void MarkFuseNodes(GraphAccessInterface::Node *gn1,
                               GraphAccessInterface::Node *gn2)
     {
         DepCompAstRefGraphNode* n1 = static_cast<DepCompAstRefGraphNode*>(gn1);
         DepCompAstRefGraphNode* n2 = static_cast<DepCompAstRefGraphNode*>(gn2);
         if (DebugRefFuse())
            std::cerr << "fusing refs: " << n1->toString() << " with " << n2->toString() << "\n";
         FuseNodeInfo &info1 = nodeMap[n1], &info2 = nodeMap[n2] ;
         assert(info2.collect == 0);
         if (info1.collect == 0) {
            info1.collect = &collect.AddCopyArray();
            info1.collect->refs.insert(n1);
         }
         info1.collect->refs.insert(n2);
         info2.collect = info1.collect;
     }
    virtual bool PreventFusion( GraphAccessInterface::Node *src,
                                   GraphAccessInterface::Node *snk,
                                   GraphAccessInterface::Edge *ge)
     {
         DepCompAstRefGraphNode* n1 = static_cast<DepCompAstRefGraphNode*>(src);
         DepCompAstRefGraphNode* n2 = static_cast<DepCompAstRefGraphNode*>(snk);
         if (DebugRefFuse())
            std::cerr << "checking fusion between " << n1->toString() << " and " << n2->toString();
         DepInfoEdge *e = static_cast<DepInfoEdge*>(ge);
         DepInfo& info = e->GetInfo();
         if (!info.IsTop() && !info.is_precise()) {
            if (DebugRefFuse())
              std::cerr << "No because of edge " << e->toString() << std::endl;
            return true;
         }
         if (DebugRefFuse())
            std::cerr << "Yes from edge " << e->toString() << std::endl;
         return false;
     }
};

LoopTreeNode* DepCompCopyArrayCollect::
OutmostCopyRoot( CopyArrayUnit& unit, DepCompAstRefGraphCreate& refDep, LoopTreeNode* treeroot)
{
 LoopTreeNode* origroot = unit.root;
 while (unit.root != 0) {
    DepCompCopyArrayCollect::CopyArrayUnit::CrossGraph crossgraph(&refDep, unit);
      GraphEdgeIterator<DepCompCopyArrayCollect::CopyArrayUnit::CrossGraph>
          edges(&crossgraph);
    if (!edges.ReachEnd())
       break;
     DepCompCopyArrayCollect::CopyArrayUnit::InsideGraph insidegraph(&refDep, unit);
     GraphEdgeIterator<DepCompCopyArrayCollect::CopyArrayUnit::InsideGraph>
          ep(&insidegraph);
     for (  ;  !ep.ReachEnd(); ++ep) {
        if (!(*ep)->GetInfo().is_precise()) {
          break;
        }
     }
     if (!ep.ReachEnd())
        break;
     LoopTreeInterface interface;
     unit.root = GetEnclosingLoop(unit.root, interface);
  }
  LoopTreeNode* res = unit.root;
  if (res == 0)
     res = treeroot;
  else if (res->LoopLevel() +1 == origroot->LoopLevel() && origroot->Parent()->ChildCount() == 1) {
     res = origroot->Parent();
  }
  unit.root = origroot;
  return res;
}


LoopTreeNode*  DepCompCopyArrayCollect:: ComputeCommonRoot(CopyArrayUnit::NodeSet& refs)
{
    LoopTreeInterface interface;
    CopyArrayUnit::NodeSet::const_iterator rp = refs.begin();

    LoopTreeNode *curroot = GetEnclosingLoop((*rp)->GetInfo().stmt, interface);
    for ( ++rp; !rp.ReachEnd(); ++rp) {
      const DepCompAstRefGraphNode* n = *rp;
      LoopTreeNode* nroot = GetCommonLoop(interface, curroot, n->GetInfo().stmt);
      if (nroot == 0) {
         curroot = get_stmtref_info().get_tree_root();
         break;
      }
      curroot = nroot;
    }
    return curroot;
}

template<class NodeIter>
bool EnforceCopyRootRemove(NodeIter nodes, const DepCompAstRefGraphNode* outnode,
                           int copylevel,
                     DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& cuts)
{
   LoopTreeInterface interface;
   bool removeall = true;
   LoopTreeNode* outstmt = outnode->GetInfo().stmt;
   for ( ; !nodes.ReachEnd(); ++nodes) {
      const DepCompAstRefGraphNode* cur = *nodes;
      if (cuts.IsMember(cur))
         continue;
      LoopTreeNode* stmt = cur->GetInfo().stmt;
      LoopTreeNode* tmp = 0;
      if (stmt == outstmt ||
          ((tmp = GetCommonLoop(interface,stmt, outstmt)) != 0 &&
             GetLoopLevel(tmp, interface) >= copylevel))  {
          cuts.insert(cur);
      }
      else
         removeall = false;
   }
   return removeall;
}

void DepCompCopyArrayToBuffer::
EnforceCopyRoot( DepCompCopyArrayCollect::CopyArrayUnit& curunit,
                 const DepCompAstRefGraphCreate& refDep,
                 const DepCompAstRefGraphNode* outnode,
                 DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& cuts)
{
    int copylevel = curunit.copylevel();

    DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphOut
           crossout(&refDep, curunit);
    GraphNodePredecessorIterator<DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphOut>
          preds(&crossout, outnode);
    bool complete = EnforceCopyRootRemove(preds, outnode, copylevel, cuts);

    DepCompCopyArrayCollect::CopyArrayUnit tmp = curunit;
    tmp.refs -= cuts;
    DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphIn crossin(&refDep, tmp);
    GraphNodeSuccessorIterator<DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphIn>
          succ(&crossin, outnode);
    if (!EnforceCopyRootRemove( succ, outnode, copylevel, cuts))
         complete = false;
    if (complete)
         return;
     tmp.refs -= cuts;
     EnforceCopyRootRemove(tmp.refs.begin(), outnode, copylevel, cuts);
}

void DepCompCopyArrayToBuffer::
EnforceCopyRoot( DepCompCopyArrayCollect::CopyArrayUnit& curunit,
                const DepCompAstRefGraphCreate& refDep,
                 DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& cuts)
{
    if (curunit.refs.size() == 1)
         return;

    if (DebugCopySplit())  {
      std::cerr << IteratorToString2( curunit.refs.begin()) << " ; \n with root = " << curunit.root->TreeToString() << std::endl;
    }

    DepCompCopyArrayCollect::CopyArrayUnit::NodeSet outnodes;
    DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphOut crossout(&refDep, curunit);
    for (
       GraphEdgeIterator<DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphOut>
         outedges(&crossout); !outedges.ReachEnd(); ++outedges) {
        DepCompAstRefGraphNode* cur = refDep.GetEdgeEndPoint(*outedges, GraphAccess::EdgeIn);
        outnodes.insert(cur);
    }
    DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphIn crossin(&refDep, curunit);
    for (
      GraphEdgeIterator<DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphIn>
          inedges (&crossin); !inedges.ReachEnd(); ++inedges) {
        DepCompAstRefGraphNode* cur = refDep.GetEdgeEndPoint(*inedges, GraphAccess::EdgeOut);
        outnodes.insert(cur);
    }

    for (DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::const_iterator p1 = outnodes.begin();
          !p1.ReachEnd(); ++p1)  {
        EnforceCopyRoot(curunit, refDep, *p1, cuts);
    }
}

void DepCompCopyArrayToBuffer::
CollectCopyArray( DepCompCopyArrayCollect& collect,
                  const DepCompAstRefGraphCreate& refDep)
{
  DepCompAstRefAnal &stmtorder = collect.get_stmtref_info();

  AstRefTypedFusionOperator fuseop(collect, &refDep);
  DepCompAstRefDAG refDag(stmtorder, &refDep);
  fuseop(&refDag);

  // AstInterface& ai = la;
  for (DepCompCopyArrayCollect::iterator arrays = collect.begin();
       arrays != collect.end(); ++arrays) {
    DepCompCopyArrayCollect::CopyArrayUnit& curunit = *arrays;
    DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& refs = curunit.refs;
    assert (refs.size() > 0);

    DepCompCopyArrayCollect::CopyArrayUnit::NodeSet cuts;
    curunit.root = collect.ComputeCommonRoot(curunit.refs);
    EnforceCopyRoot(curunit,refDep,cuts);

    if (cuts.size() > 0 && cuts.size() < refs.size()) {
       refs -= cuts;
       collect.AddCopyArray().refs = cuts;
    }
  }
}

CopyArrayConfig
ComputeCopyConfig( const DepCompAstRefAnal& stmtorder,
                   const DepCompCopyArrayCollect::CopyArrayUnit& unit,
                   const DepCompAstRefGraphNode* initcut)
{
    assert (unit.refs.size() > 0);

    AstInterface& ai = LoopTransformInterface::getAstInterface();
    const DepCompAstRef& initInfo = initcut->GetInfo();

    AstNodePtr lhs;
    bool readlhs = false;
    bool is_init = ai.IsAssignment(initInfo.stmt->GetOrigStmt(), &lhs,0,&readlhs)
                   && !readlhs && (lhs == initcut->GetInfo().orig);
    AstNodeType inittype;
    if (ai.IsExpression(initInfo.orig, &inittype)==AST_NULL)
      assert(false);
    bool has_write = false;

    AstNodePtr arr;
    std::string arrname, elemtypename;
    ai.GetTypeInfo(inittype, 0, &elemtypename); //QY: strip out ref info &
    AstNodeType elemtype = ai.GetType(elemtypename);

    AstInterface::AstNodeList initIndex;
    if (!LoopTransformInterface::IsArrayAccess(initInfo.orig, &arr, &initIndex) || !ai.IsVarRef(arr,0,&arrname))
       assert(false);

    SelectArray cursel(elemtype, arrname, initIndex.size());
    cursel.select(initInfo.stmt, unit.root, initIndex);

    for (DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::const_iterator p = unit.refs.begin();
          !p.ReachEnd(); ++p)  {
        const DepCompAstRefGraphNode* curref = *p;
        const DepCompAstRef& curinfo = curref->GetInfo();
        if (stmtorder.is_mod_ref(curinfo.orig))
            has_write = true;
        AstInterface::AstNodeList curIndex;
        if (!LoopTransformInterface::IsArrayAccess(curinfo.orig, 0, &curIndex))
           assert(false);
        if (cursel.select(curinfo.stmt, unit.root, curIndex))
          is_init = false;
   }
   cursel.set_bufsize(ai);
   LoopTreeNode* shift = 0;
   if (unit.carrybyroot)
       shift = unit.root;
   int copyopt = 0;
   if (!is_init)
      copyopt |= INIT_COPY;
   if  (has_write)
      copyopt |= SAVE_COPY;

   CopyArrayOpt opt = (CopyArrayOpt)copyopt;
   CopyArrayConfig curconfig(ai, cursel, opt, shift);
   return curconfig;
}

void ComputeCutBoundary( const DepCompAstRefGraphCreate& refDep,
                         const DepCompAstRefAnal& stmtorder,
                         DepCompCopyArrayCollect::CopyArrayUnit& unit,
                         const DepCompAstRefGraphNode*& cut1,
                         const DepCompAstRefGraphNode*& cut2)
{
   cut1 = cut2 = 0;

   for ( DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::const_iterator p = unit.refs.begin();
          !p.ReachEnd(); ++p)  {
      const DepCompAstRefGraphNode* curref = *p;
      if (cut1 == 0 || stmtorder.CompareAstRef(cut1->GetInfo(), curref->GetInfo()) > 0)
           cut1 = curref;
      if (cut2 == 0 || stmtorder.CompareAstRef(cut2->GetInfo(), curref->GetInfo()) < 0)
           cut2 = curref;
   }
   assert(cut1 != 0 && cut2 != 0);
}



void DepCompCopyArrayToBuffer::
ApplyCopyArray( DepCompCopyArrayCollect& collect,
                 const DepCompAstRefGraphCreate& refDep)
{
   const DepCompAstRefAnal& stmtorder = collect.get_stmtref_info();
   for (DepCompCopyArrayCollect::iterator arrays = collect.begin();
        arrays != collect.end(); ++arrays) {
      DepCompCopyArrayCollect::CopyArrayUnit& curarray = *arrays;
      if (DebugCopyConfig())
        std::cerr << IteratorToString2(curarray.refs.begin()) << std::endl;

      const DepCompAstRefGraphNode* initcut = 0, *savecut = 0;
      ComputeCutBoundary(refDep, stmtorder, curarray, initcut, savecut);
      CopyArrayConfig curconfig =
                 ComputeCopyConfig(stmtorder, curarray, initcut);

      LoopTreeNode* repl = curarray.root;
assert(repl != 0);
      LoopTreeNode* init = initcut->GetInfo().stmt;
      LoopTreeNode* save = savecut->GetInfo().stmt;
      CopyArrayOpt opt = curconfig.get_opt();
      if (init != 0) {
         if (opt & SHIFT_COPY)
            init = curarray.root;
         else for ( ; init->Parent() != curarray.root;
                    init=init->Parent());
      }
      if ( (opt & SHIFT_COPY) && (init != 0 || save != 0))
            save = curarray.root->LastChild();
      else if (save != 0)
            for (; save->Parent()!=curarray.root;
              save=save->Parent());
      ApplyXform(curarray, curconfig, repl, init,save);
  }
}

void DepCompCopyArrayToBuffer::ApplyXform(
                DepCompCopyArrayCollect::CopyArrayUnit& curarray,
                CopyArrayConfig& curconfig, LoopTreeNode* replRoot,
                LoopTreeNode* initstmt, LoopTreeNode* savestmt)
{
      AstInterface & fa = LoopTransformInterface::getAstInterface();
      curconfig.get_arr().set_bufname(fa);
      LoopTreeCopyArrayToBuffer()( replRoot,initstmt, savestmt, curconfig);

      for ( DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::const_iterator p
              = curarray.refs.begin();
            !p.ReachEnd(); ++p)  {
          const DepCompAstRef& curinfo = (*p)->GetInfo();
          AstNodePtr curref = curinfo.orig;
          AstInterface::AstNodeList index;
          if (!LoopTransformInterface::IsArrayAccess(curref, 0,&index))
              assert(false);
          AstNodePtr currepl = curconfig.get_arr().buf_codegen(fa, index);
          LoopTreeReplaceAst()(curinfo.stmt, curref, currepl);
      }
}
