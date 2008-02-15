
#include <stdlib.h>
#include <sstream>

#include <general.h>
#include <CommandOptions.h>
#include <LoopInfoInterface.h>
#include <DepCompTransform.h>
#include <LoopTreeTransform.h>
#include <SCCAnalysis.h>
#include <TypedFusion.h>
#include <union_find.h>
#include <GraphScope.h>
#include <GraphIO.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a header file it is OK here!
#define Boolean int

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
      result.Add(s);
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
  typedef PtrSetWrap<LoopTreeNode> SelectLoopTreeNode;
  SelectLoopTreeNode stmts1, stmts2;

  LoopTreeDepCompSubtree loopComp( tc, l);
  DepLinkedNodes( loopComp, stmts, GraphAccess::EdgeIn, stmts1);
  DepLinkedNodes( loopComp, stmts, GraphAccess::EdgeOut, stmts2);

  stmts2 &= stmts1;
  stmts1 -= stmts2;

  if (stmts1.NumberOfEntries()) {
    SelectPtrSet<LoopTreeNode> sel1(stmts1);
    LoopTreeDistributeNode()(l, sel1);
  }
 SelectPtrSet<LoopTreeNode> sel2(stmts2);
  return Result( LoopTreeDistributeNode()(l, sel2), stmts2);
}

DepCompDistributeLoop::Result DepCompDistributeLoop ::
operator() ( LoopTreeDepComp &tc, LoopTreeNode *l)
{
  typedef PtrSetWrap<LoopTreeNode> SelectLoopTreeNode;

  LoopTreeDepCompSubtree loopComp( tc, l);
  Result result;

  LoopTreeDepGraph *depGraph = loopComp.GetDepGraph();
  SCCGraphCreate sccGraph( depGraph );
  sccGraph.TopoSort();

  for ( GroupGraphCreate::NodeIterator sccIter = sccGraph.GetNodeIterator();
       !sccIter.ReachEnd(); sccIter++) {
    GroupGraphNode *scc = sccIter.Current(); 
    SelectLoopTreeNode treeSet;
    for (GroupGraphNode::Iterator iter=scc->GetIterator(); !iter.ReachEnd(); iter++) {
       LoopTreeDepGraphNode *n = 
          static_cast<LoopTreeDepGraphNode*>(iter.Current()); 
       treeSet.Add( tc.GetTreeNode(n) );
    }
    if (treeSet.NumberOfEntries()) {
      SelectPtrSet<LoopTreeNode> sel(treeSet);
      LoopTreeNode* tmp = LoopTreeDistributeNode()( l, sel);
      if (result.node == 0) {
         result.node = tmp; result.sel = treeSet;
      } 
    }
  }
  return result;
}

class DepCompAstRefDAG : public DepCompAstRefGraphCreate 
{
public:
  DepCompAstRefDAG(const DepCompAstRefAnal& stmtorder, const DepCompAstRefGraphCreate* g) 
   {
      DoublyLinkedListWrap <DepCompAstRefGraphNode*> nodelist;
      for (DepCompAstRefGraphCreate::NodeIterator nodes = g->GetNodeIterator();
            !nodes.ReachEnd(); nodes.Advance()) {
           DepCompAstRefGraphNode* n = nodes.Current();
           CreateBaseNode(n);
           nodelist.AppendLast(n);
      }
      if (nodelist.size() <= 1)
          return;
      for (DepCompAstRefGraphCreate::NodeIterator nodes = g->GetNodeIterator();
            !nodes.ReachEnd(); nodes.Advance()) {
         DepCompAstRefGraphNode* n = *nodes;
         DepCompAstRef& info = n->GetInfo();
         for (DepInfoEdgeIterator edges = g->GetNodeEdgeIterator(n, GraphAccess::EdgeOut);
               !edges.ReachEnd(); ++edges) {
             DepInfoEdge* e = *edges;
             DepCompAstRefGraphNode* n1 = g->GetEdgeEndPoint(e, GraphAccess::EdgeIn);
             DepCompAstRef& info1 = n1->GetInfo();
             int c = stmtorder.CompareAstRef(info,info1);
             if (c < 0) {
                   CreateBaseEdge(n, n1, e);
             }
             else if (c > 0) {
                  CreateBaseEdge(n1,n,e);
             }
         }
      }   
      if (DebugRefFuse()) {
         cerr << GraphToString(*this) << endl;
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
    map<DepCompAstRefGraphNode*,FuseNodeInfo> nodeMap;
    int size;
    LoopTransformInterface& la;
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
              cerr << "mapping node " << n1->ToString() << " to " << size << endl;
            nodeMap[n1] = FuseNodeInfo(size);
            AddNodeType(g, n1);
         }
       }
     }
  public:
    AstRefTypedFusionOperator(LoopTransformInterface& _la, DepCompCopyArrayCollect& c,
                              const DepCompAstRefGraphCreate* g) 
         : la(_la), collect(c)
      {
        size = 0;
        for (DepCompAstRefGraphCreate::NodeIterator nodes = g->GetNodeIterator();
            !nodes.ReachEnd(); nodes.Advance()) {
           DepCompAstRefGraphNode* n = nodes.Current();
           if (nodeMap.find(n) != nodeMap.end()) 
              continue;
           AstNodePtr r = n->GetInfo().orig;
           AstNodePtr arr;
           if (la.IsArrayAccess(r, &arr) && AstInterface::IsVarRef(arr))  {
              if (DebugRefFuse())
                  cerr << "mapping node " << n->ToString() << " to " << size << endl;
              nodeMap[n] = size;
              AddNodeType(g, n);
              ++size;
           }
       }
      }
    void operator()(DepCompAstRefDAG* dag)
      {
        for (int i = 0; i < size; ++i) {
           if (DebugRefFuse()) 
              cerr << "fusing node type " << i << endl;
           TypedFusion()(dag, *this, i);
        }
        for (DepCompAstRefGraphCreate::NodeIterator nodes = dag->GetNodeIterator();
            !nodes.ReachEnd(); nodes.Advance()) {
           DepCompAstRefGraphNode* n1 = *nodes;
           FuseNodeInfo &info1 = nodeMap[n1];
           if (info1.collect == 0) {
              info1.collect = &collect.AddCopyArray();
              info1.collect->refs.Add(n1);
           }
        }
      }
    virtual int GetNodeType( GraphNode *n) 
      {
         DepCompAstRefGraphNode* n1 = dynamic_cast<DepCompAstRefGraphNode*>(n);
         assert(n1 != 0);
         if (nodeMap.find(n1) == nodeMap.end()) {
            return nodeMap.size(); 
         }
         else {
            return nodeMap[n1].nodetype;
         }
      }
    virtual void MarkFuseNodes( GraphNode *gn1, GraphNode *gn2) 
     {
         DepCompAstRefGraphNode* n1 = dynamic_cast<DepCompAstRefGraphNode*>(gn1);
         assert(n1 != 0);
         DepCompAstRefGraphNode* n2 = dynamic_cast<DepCompAstRefGraphNode*>(gn2);
         assert(n2 != 0);
         if (DebugRefFuse())
            cerr << "fusing refs: " << n1->ToString() << " with " << n2->ToString() << "\n";
         FuseNodeInfo &info1 = nodeMap[n1], &info2 = nodeMap[n2] ;
         assert(info2.collect == 0);
         if (info1.collect == 0) {
            info1.collect = &collect.AddCopyArray();
            info1.collect->refs.Add(n1);
         }
         info1.collect->refs.Add(n2);
         info2.collect = info1.collect;
     }
    virtual Boolean PreventFusion( GraphNode *src, GraphNode *snk,
                                   GraphEdge *ge) 
     {
         DepCompAstRefGraphNode* n1 = dynamic_cast<DepCompAstRefGraphNode*>(src);
         assert(n1 != 0);
         DepCompAstRefGraphNode* n2 = dynamic_cast<DepCompAstRefGraphNode*>(snk);
         assert(n2 != 0);
         if (DebugRefFuse())
            cerr << "checking fusion between " << n1->ToString() << " and " << n2->ToString();
         DepInfoEdge *e = dynamic_cast<DepInfoEdge*>(ge);
         assert(e != 0);
         DepInfo& info = e->GetInfo();
         if (!info.IsTop() && !info.is_precise()) {
            if (DebugRefFuse())
              cerr << "No because of edge " << e->ToString() << endl;
            return true;
         }
         if (DebugRefFuse())
            cerr << "Yes from edge " << e->ToString() << endl;
         return false;
     }
};

LoopTreeNode* DepCompCopyArrayCollect:: 
OutmostCopyRoot( CopyArrayUnit& unit, DepCompAstRefGraphCreate& refDep, LoopTreeNode* treeroot)
{
 LoopTreeNode* origroot = unit.root;
 while (unit.root != 0) {
    DepCompCopyArrayCollect::CopyArrayUnit::CrossGraph crossgraph(&refDep, unit);
    DepInfoEdgeIterator edges =
      GraphGetEdgeIterator<DepCompCopyArrayCollect::CopyArrayUnit::CrossGraph>
                 ()(&crossgraph);
    if (!edges.ReachEnd()) {
       break; 
    }
     DepCompCopyArrayCollect::CopyArrayUnit::InsideGraph insidegraph(&refDep, unit);
     DepInfoEdgeIterator ep =
         GraphGetEdgeIterator<DepCompCopyArrayCollect::CopyArrayUnit::InsideGraph>
                 ()(&insidegraph);
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
    CopyArrayUnit::NodeSet::Iterator rp = refs.GetIterator();

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
          cuts.Add(cur);
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
    typedef GraphGetNodePredecessors<DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphOut>
            GraphGetPredIterator;
    typedef GraphGetNodeSuccessors<DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphIn>
            GraphGetSuccIterator;

    DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphOut crossout(&refDep, curunit);
    bool complete = EnforceCopyRootRemove( GraphGetPredIterator()(&crossout, outnode),
                                           outnode, copylevel, cuts);

    DepCompCopyArrayCollect::CopyArrayUnit tmp = curunit;
    tmp.refs -= cuts;
    DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphIn crossin(&refDep, tmp);
    if (!EnforceCopyRootRemove( GraphGetSuccIterator()(&crossin, outnode),
                                           outnode, copylevel, cuts))
         complete = false;
    if (complete)
         return; 
     tmp.refs -= cuts;
     EnforceCopyRootRemove(tmp.refs.GetIterator(), outnode, copylevel, cuts);
}

void DepCompCopyArrayToBuffer::
EnforceCopyRoot( DepCompCopyArrayCollect::CopyArrayUnit& curunit, 
                const DepCompAstRefGraphCreate& refDep,
                 DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& cuts)
{
    if (curunit.refs.size() == 1)
         return;

    if (DebugCopySplit())  {
      cerr << IteratorToString2( curunit.refs.GetIterator()) << " ; \n with root = " << curunit.root->TreeToString() << endl;
    }
    
    DepCompCopyArrayCollect::CopyArrayUnit::NodeSet outnodes; 
    DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphOut crossout(&refDep, curunit);
    for (DepInfoEdgeIterator outedges = 
         GraphGetEdgeIterator<DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphOut>()(&crossout); 
         !outedges.ReachEnd(); ++outedges) {
        DepCompAstRefGraphNode* cur = refDep.GetEdgeEndPoint(*outedges, GraphAccess::EdgeIn);
        outnodes.Add(cur);
    }
    DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphIn crossin(&refDep, curunit);
    for (DepInfoEdgeIterator inedges = 
          GraphGetEdgeIterator<DepCompCopyArrayCollect::CopyArrayUnit::CrossGraphIn>()(&crossin); 
         !inedges.ReachEnd(); ++inedges) {
        DepCompAstRefGraphNode* cur = refDep.GetEdgeEndPoint(*inedges, GraphAccess::EdgeOut);
        outnodes.Add(cur);
    }
    
    for (DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::Iterator p1 = outnodes.GetIterator();
          !p1.ReachEnd(); ++p1)  {
        EnforceCopyRoot(curunit, refDep, *p1, cuts);
    }
}

void DepCompCopyArrayToBuffer::
CollectCopyArray( LoopTransformInterface& la, DepCompCopyArrayCollect& collect,
                  const DepCompAstRefGraphCreate& refDep)
{
  DepCompAstRefAnal &stmtorder = collect.get_stmtref_info();

  AstRefTypedFusionOperator fuseop(la, collect, &refDep);
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

    if (cuts.size() > 0) {
       assert(cuts.size() < refs.size());
       refs -= cuts;
       collect.AddCopyArray().refs = cuts;
    }
  }
} 

CopyArrayConfig 
ComputeCopyConfig( LoopTransformInterface& la, const DepCompAstRefAnal& stmtorder,
                   const DepCompCopyArrayCollect::CopyArrayUnit& unit,
                   const DepCompAstRefGraphNode* initcut, int& copyopt) 
{
    assert (unit.refs.size() > 0);

    AstInterface& ai = la;
    const DepCompAstRef& initInfo = initcut->GetInfo();

    AstNodePtr lhs;
    bool is_init = AstInterface::IsAssignment(initInfo.stmt->GetOrigStmt(), &lhs) 
                   && (lhs == initcut->GetInfo().orig);
    bool has_write = false;

    AstNodePtr arr;
    string arrname, elemtypename;
    ai.GetTypeInfo(ai.GetExpressionType(initInfo.orig), 0, &elemtypename);
    AstNodeType elemtype = ai.GetType(elemtypename);

    AstInterface::AstNodeList initIndex;
    if (!la.IsArrayAccess(initInfo.orig, &arr, &initIndex) || !ai.IsVarRef(arr,0,&arrname))
       assert(false);

    SelectArray cursel(initIndex.size());
    cursel.select(la, initInfo.stmt, unit.root, initIndex);

    for (DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::Iterator p = unit.refs.GetIterator(); 
          !p.ReachEnd(); ++p)  {
        const DepCompAstRefGraphNode* curref = *p;
        const DepCompAstRef& curinfo = curref->GetInfo();
        if (stmtorder.is_mod_ref(curinfo.orig))
            has_write = true;
        AstInterface::AstNodeList curIndex;
        if (!la.IsArrayAccess(curinfo.orig, 0, &curIndex))
           assert(false);
        if (cursel.select(la, curinfo.stmt, unit.root, curIndex)) 
          is_init = false;
   }
   LoopTreeNode* shift = 0;
   if (unit.carrybyroot)
       shift = unit.root; 
   CopyArrayConfig curconfig(ai, arrname, elemtype, cursel, shift);
   if (DebugCopyConfig()) 
         cerr << "copy config " << curconfig.ToString() << " : " << endl;

   if (!is_init) 
      copyopt |= CopyArrayConfig::INIT_COPY;
   if  (has_write)
      copyopt |= CopyArrayConfig::SAVE_COPY;
   if  (curconfig.shift_buffer())
      copyopt |= CopyArrayConfig::SHIFT_COPY;
   if (curconfig.need_allocate_buffer())
      copyopt |= CopyArrayConfig::ALLOC_COPY;
   if (curconfig.need_delete_buffer())
      copyopt |= CopyArrayConfig::DELETE_COPY;
      
   return curconfig;
}

void ComputeCutBoundary( const DepCompAstRefGraphCreate& refDep,
                         const DepCompAstRefAnal& stmtorder,
                         DepCompCopyArrayCollect::CopyArrayUnit& unit, 
                         const DepCompAstRefGraphNode*& cut1, 
                         const DepCompAstRefGraphNode*& cut2)
{
   cut1 = cut2 = 0;

   for ( DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::Iterator p = unit.refs.GetIterator(); 
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
ApplyCopyArray( LoopTransformInterface& la, DepCompCopyArrayCollect& collect,
                 const DepCompAstRefGraphCreate& refDep)
{
   const DepCompAstRefAnal& stmtorder = collect.get_stmtref_info();
   for (DepCompCopyArrayCollect::iterator arrays = collect.begin();
        arrays != collect.end(); ++arrays) {
      DepCompCopyArrayCollect::CopyArrayUnit& curarray = *arrays;
      if (DebugCopyConfig())
        cerr << IteratorToString2(curarray.refs.GetIterator()) << endl;


      const DepCompAstRefGraphNode* initcut = 0, *savecut = 0;
      ComputeCutBoundary(refDep, stmtorder, curarray, initcut, savecut);
      int copyopt = 0; 
      CopyArrayConfig curconfig = 
                 ComputeCopyConfig(la, stmtorder, curarray, initcut,copyopt);

      LoopTreeNode* initstmt = initcut->GetInfo().stmt;
      LoopTreeNode* savestmt = savecut->GetInfo().stmt;
      if (initstmt != 0) {
         if (copyopt & CopyArrayConfig::SHIFT_COPY)
            initstmt = curarray.root;
         else 
            for ( ; initstmt->Parent() != curarray.root;
                    initstmt=initstmt->Parent()) {}
      }
      if ( (copyopt & CopyArrayConfig::SHIFT_COPY)
            && (initstmt != 0 || savestmt != 0)) {
            savestmt = curarray.root->LastChild();
      }
      else if (savestmt != 0)  {
            for (; savestmt->Parent()!=curarray.root; 
                    savestmt=savestmt->Parent()) {}
      }

      if (DebugCopyConfig() && initstmt != 0) 
           cerr << "init cutting node: " << initstmt->ToString() << endl;
      if (DebugCopyConfig() && savestmt != 0) 
           cerr << "save cutting node: " << savestmt->ToString() << endl;
      if (DebugCopyConfig())
           cerr << "CopyOpt = " << CopyArrayConfig::CopyOpt2String(copyopt) << endl;

      LoopTreeCopyArrayToBuffer()(la, initstmt, savestmt, curconfig, copyopt);

      AstInterface & fa = la;
      for ( DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::Iterator p 
              = curarray.refs.GetIterator(); 
            !p.ReachEnd(); ++p)  {
          const DepCompAstRef& curinfo = (*p)->GetInfo();
          AstNodePtr curref = curinfo.orig;
          AstInterface::AstNodeList index;
          if (!la.IsArrayAccess(curref, 0,&index)) 
              assert(false);
          AstNodePtr currepl = curconfig.buf_codegen(fa, index);
          LoopTreeReplaceAst()(la,  curinfo.stmt, curref, currepl); 
      } 
   }
  }
