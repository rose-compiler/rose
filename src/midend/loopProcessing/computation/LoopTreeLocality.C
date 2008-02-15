
#include <stdlib.h>

#include <sstream>
#include <general.h>
#include <LoopTreeLocality.h>
#include <ReuseAnalysis.h>
#include <DepGraphTransform.h>
#include <CommandOptions.h>
#include <StmtInfoCollect.h>
#include <StmtDepAnal.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a header file it is OK here!
#define Boolean int

bool DebugRefGraph()
{
  static int r = 0;
  if (r == 0) {
     r = CmdOptions::GetInstance()->HasOption("-debugrefgraph")? 1 : -1; 
  }
  return r == 1;
}

LoopTreeLocalityAnal :: 
 LoopTreeLocalityAnal( LoopTransformInterface& _fa, LoopTreeDepCompCreate& c)
    : comp(c), anal(comp.GetDepAnal()),
      inputCreate(c.GetTreeNodeMap()), fa(_fa)
{
  comp.GetLoopTreeCreate()->AttachObserver(inputCreate);
  LoopTreeNode* root = comp.GetLoopTreeRoot();
  DepCompAstRefAnal stmtorder(_fa, root);
  for (LoopTreeTraverseSelectStmt p1(root); !p1.ReachEnd(); ++p1) {
    LoopTreeDepGraphNode *n1 = comp.GetDepNode(p1.Current());
    for (LoopTreeTraverseSelectStmt p2 = p1; !p2.ReachEnd(); ++p2) {
       LoopTreeDepGraphNode *n2 = comp.GetDepNode(p2.Current());
       ComputeInputDep(n1, n2, stmtorder);
    }
  }
}

LoopTreeLocalityAnal :: ~LoopTreeLocalityAnal()
{
  comp.GetLoopTreeCreate()->DetachObserver(inputCreate);
}

void LoopTreeLocalityAnal ::
ComputeInputDep( LoopTreeDepGraphNodeIterator nodeIter, DepCompAstRefAnal& stmtorder)
{
  SinglyLinkedListWrap <LoopTreeDepGraphNode*> nodeSet;
  for ( ; ! nodeIter.ReachEnd() ; nodeIter.Advance()) {
     LoopTreeDepGraphNode *n = nodeIter.Current();
     for (SinglyLinkedListWrap<LoopTreeDepGraphNode*>::Iterator nodeIter1(nodeSet);
          !nodeIter1.ReachEnd(); ++nodeIter1) {
         LoopTreeDepGraphNode *n1 = *nodeIter1; 
         ComputeInputDep( n, n1, stmtorder);
     }
     nodeSet.AppendLast(n);
  }
}

void LoopTreeLocalityAnal ::
ComputeInputDep( LoopTreeDepGraphNode *n1, LoopTreeDepGraphNode *n2, DepCompAstRefAnal& stmtorder)
{
  if (!inputCreate.ContainNode(n1)) 
    inputCreate.AddNode(n1);
  if (!inputCreate.ContainNode(n2))
    inputCreate.AddNode(n2);
  if (stmtorder.CompareStmt(comp.GetTreeNode(n1),comp.GetTreeNode(n2)) > 0) {
    LoopTreeDepGraphNode * tmp = n1;
    n1 = n2; n2 = tmp; 
  }
  if (GraphGetCrossEdge<LoopTreeDepGraph>()(&inputCreate,n1,n2) != 0)
    return;
  
  inputCreate.BuildDep(fa, anal, n1,n2,DEPTYPE_INPUT);
}

void LoopTreeLocalityAnal ::
ComputeInputDep( LoopTreeDepGraphNodeIterator srcIter,
                 LoopTreeDepGraphNodeIterator snkIter, DepCompAstRefAnal& stmtorder)
{
  for ( LoopTreeDepGraphNode *n; (n = srcIter.Current()); srcIter.Advance()) {
     snkIter.Reset();
     for (LoopTreeDepGraphNode *n1; (n1 = snkIter.Current()); snkIter++) {
         ComputeInputDep( n, n1, stmtorder);
     }
  }
}

class AccumulateSpatialReuse : public CollectObject<AstNodePtr>
{
  float res;
  LoopTransformInterface& la;
  string ivarname;
  int linesize;
 public:
  Boolean operator()(const AstNodePtr& cur)
  { res +=  SelfSpatialReuse( la, cur, ivarname, linesize); return true;}
  AccumulateSpatialReuse( LoopTransformInterface& _la, const string& _ivarname, int _linesize)
    : res(0), la(_la), ivarname(_ivarname), linesize(_linesize) {}
  float get_result() const { return res; }
};

float LoopTreeLocalityAnal ::
SelfSpatialReuses( LoopTreeNode *n, int loop, int linesize)
{
   int loop1 = comp.GetDepNode(n)->LoopTreeDim2AstTreeDim(loop);
   AstNodePtr s = n->GetOrigStmt();
   string name = anal.GetStmtInfo(fa, s).ivars[loop1].GetVarName();
   AccumulateSpatialReuse  col(fa, name, linesize);
   AnalyzeStmtRefs( fa, s, col, col);
   return col.get_result();
}

class MapSrcSinkLooplevel 
  : public Map2Object<AstNodePtr, DepDirection,int>
{
  int srclevel, snklevel;
  int operator() (const AstNodePtr&, const DepDirection& dir)
   {
     return (dir == DEP_SRC)? srclevel : snklevel;
   }
 public:
  MapSrcSinkLooplevel(int _srclevel, int _snklevel) 
    : srclevel(_srclevel), snklevel(_snklevel) {}
};

int LoopTreeLocalityAnal ::
TemporaryReuseRefs(LoopTreeNode *s1, int loop1, LoopTreeNode *s2, int loop2, 
                   AstNodeSet &refSet, int reuseDist) 
{
  LoopTreeDepGraphNode *n1=comp.GetDepNode(s1), *n2=comp.GetDepNode(s2);

  LoopTreeDepGraph* graph = comp.GetDepGraph();
  DepInfoEdgeIterator iter1 = GraphGetCrossEdgeIterator<LoopTreeDepGraph>()(graph,n1,n2); 
  DepInfoEdgeIterator iter2 = GraphGetCrossEdgeIterator<LoopTreeDepGraph>()(&inputCreate,n1,n2);
  MultiIterator<DepInfo,DepInfo,DepInfoEdgeConstInfoIterator> 
          iter(iter1,iter2);
  IteratorImplTemplate<DepInfo, MultiIterator<DepInfo,DepInfo,DepInfoEdgeConstInfoIterator> >
      iter3(iter);
  MapSrcSinkLooplevel loopmap(loop1, loop2);

  AppendSTLSet<AstNodePtr> col(refSet);
  ::TemporaryReuseRefs( iter3, loopmap, col, &reuseDist);
  return refSet.size();
}

bool operator < (const DepCompAstRef& n1, const DepCompAstRef& n2) 
   { 
      return n1.ToHandle() < n2.ToHandle();
   }


string DepCompAstRef::ToHandle() const 
{
   stringstream res;
   res <<  stmt  << orig;
   return res.str();
}

string DepCompAstRef::ToString() const 
{
  stringstream out;
  out << AstInterface::AstToString(orig) << " : " << stmt << ":" << stmt->ToString(); 
  return out.str();
}

DepCompAstRefGraphNode* DepCompAstRefGraphCreate::CreateNode(const AstNodePtr& r, LoopTreeNode* stmt)
{
    DepCompAstRef cur(r, stmt);
    AstRefNodeMap::const_iterator rp = refNodeMap.find(cur);

    DepCompAstRefGraphNode* rnode = 0;
    if (rp == refNodeMap.end()) {
       rnode = new DepCompAstRefGraphNode(this, cur);
       CreateBaseNode(rnode);
       refNodeMap[cur] = rnode;
    }
    else
       rnode=  (*rp).second;
    return rnode;
}

DepInfoEdge*  DepCompAstRefGraphCreate:: 
CreateEdge( DepCompAstRefGraphNode* src,  DepCompAstRefGraphNode* snk, const DepInfo& dep)
{
   DepInfoEdge *e = new DepInfoEdge(this, dep); 
   CreateBaseEdge(src, snk, e);
   if (DebugRefGraph()) {
      cerr << "creating edge in ref graph: " << src->ToString() << "->" << snk->ToString() << " : " << e->ToString() << endl;
   }
   return e;
}

template<class Graph>
void CreateEdge( LoopTransformInterface& la, DepCompAstRefGraphCreate* res, 
            LoopTreeDepComp& comp, Graph* g, LoopTreeDepGraphCreate::EdgeIterator deps)
{
  for ( ; !deps.ReachEnd(); ++deps) {
    DepInfoEdge *cur = deps.Current();
    const DepInfo& curdep = cur->GetInfo();
    LoopTreeDepGraphNode *depsrc = g->GetEdgeEndPoint(cur,GraphAccess::EdgeOut);
    LoopTreeDepGraphNode *depsnk = g->GetEdgeEndPoint(cur,GraphAccess::EdgeIn);
    AstNodePtr src = curdep.SrcRef(), snk = curdep.SnkRef();
    if (src == 0 && snk == 0)
       continue;
    if (la.IsArrayAccess(src) || la.IsArrayAccess(snk)) {
       DepCompAstRefGraphNode* srcnode = res->CreateNode(src,comp.GetTreeNode(depsrc));
       DepCompAstRefGraphNode* snknode = res->CreateNode(snk,comp.GetTreeNode(depsnk));;
       res->CreateEdge( srcnode, snknode, curdep);
    }
  }
}

void DepCompAstRefGraphCreate::
Build(LoopTransformInterface& la, LoopTreeLocalityAnal& tc, LoopTreeNode* root) 
{ 
  LoopTreeDepComp& comp = tc.GetDepComp();
  LoopTreeNodeDepMap m = comp.GetTreeNodeMap();

  LoopTreeDepGraphCreate::SubtreeGraph depgraph = 
            LoopTreeDepGraphCreate::GetSubtreeGraph(comp.GetDepGraph(), m, root);

  ::CreateEdge( la, this, comp, &depgraph, 
              GraphGetEdgeIterator<LoopTreeDepGraphCreate::SubtreeGraph>()(&depgraph));

  LoopTreeDepGraphCreate::SubtreeGraph inputgraph =             
            LoopTreeDepGraphCreate::GetSubtreeGraph(tc.GetInputGraph(), m, root);
  ::CreateEdge( la, this, comp, &inputgraph, 
                GraphGetEdgeIterator<LoopTreeDepGraphCreate::SubtreeGraph>()(&inputgraph));
}
     
void DepCompAstRefAnal:: Append( LoopTransformInterface& ai, LoopTreeNode* root)
     {
       class CollectModRef : public CollectObject<pair<AstNodePtr,AstNodePtr> >  
        { 
         map<AstNodePtr,int>& refmap;
         public:
         CollectModRef(map<AstNodePtr,int>&  m) : refmap(m){}
         Boolean operator()(const pair<AstNodePtr,AstNodePtr>& cur)
          {
            if (refmap.find(cur.first) == refmap.end()) {
                int num = refmap.size();
                refmap[cur.first] = -num;
                return true;
            }
            return false;
          }
       } modcollect(refmap);
       class CollectReadRef : public CollectObject<pair<AstNodePtr,AstNodePtr> >  
        { 
         map<AstNodePtr,int>& refmap;
         public:
         CollectReadRef(map<AstNodePtr,int>&  m) : refmap(m){}
         Boolean operator()(const pair<AstNodePtr,AstNodePtr>& cur)
          {
            if (refmap.find(cur.first) == refmap.end()) {
                int num = refmap.size();
                refmap[cur.first] = num;
                return true;
            }
             return false;
          }
       } readcollect(refmap);
       int stmtnum = stmtmap.size();
       for (LoopTreeTraverse stmts(root, LoopTreeTraverse::PostOrder); 
            !stmts.ReachEnd(); stmts.Advance(),++stmtnum) {
           LoopTreeNode* curstmt = stmts.Current();
           assert(stmtmap.find(curstmt) == stmtmap.end());
           stmtmap[curstmt] = stmtnum; 
           StmtSideEffectCollect(ai.getSideEffectInterface())(ai, curstmt->GetOrigStmt(), &modcollect, &readcollect);  
       } 
     }

Boolean DepCompAstRefGraphCreate::
SelfReuseLevel( const DepCompAstRefGraphNode* n, int level) const
{
  for (DepInfoEdgeIterator edges = 
        GraphGetCrossEdgeIterator<DepCompAstRefGraphCreate>()(this, n,n);
       !edges.ReachEnd(); ++edges) {
    int cur = (*edges)->GetInfo().CarryLevel();
    if (cur == level)
         return true;
  } 
  return false;
}

/*
void DepCompAstRefGraphCreate::SplitNodeDomain( DepCompAstRefGraphNode* n) 
{
  typedef  CopySplitNodeEdge<DepCompAstRefGraphCreate> CopySplitOperator;
  typedef  UpdateSplitNodeEdge<DepCompAstRefGraphCreate> UpdateSplitOperator;
  PtrSetWrap<DepCompAstRefGraphNode> connects;
  for (int i = 0; i < 2; ++i) {
    GraphAccess::EdgeDirection d = (i == 0)? GraphAccess::EdgeOut : GraphAccess::EdgeIn;
    DepCompAstRefGraphCreate::EdgeIterator edges = GetNodeEdgeIterator(n, d);
    for ( ; !edges.ReachEnd(); ++edges) {
      DepInfoEdge* e = (*edges);
      if (!e->GetInfo().is_precise())
         continue;
      DepCompAstRefGraphNode* n1 = GetEdgeEndPoint(e, GraphAccess::Reverse(d)); 
      connects.Add(n1);
    }
  }
  set<DomainCond> splits;
  for (PtrSetWrap<DepCompAstRefGraphNode>::Iterator p = connects.GetIterator();
       !p.ReachEnd(); p.Advance()) {
      DepCompAstRefGraphNode* cur = (*p);
      PtrSetWrap<DepCompAstRefGraphNode>::Iterator p1 = p; 
      for (p1.Advance(); !p1.ReachEnd(); p1.Advance()) {
          DepCompAstRefGraphNode* cur1 = (*p1);
          if (cur->GetInfo().stmt != cur1->GetInfo().stmt &&
              GetEdge(cur,cur1) == 0 && GetEdge(cur1,cur) == 0) {
             DepInfoEdge *e = GetEdge(n,cur);
             DomainCond c; 
             if (e != 0)
                c = DomainCond(e->GetInfo(), DEP_SRC);
             else { 
                DepInfoEdge *e1 = GetEdge(cur,n);
                assert(e1 != 0);
                c = DomainCond(e1->GetInfo(), DEP_SINK);
             }
             splits.insert(c);
             break;
          }
      }
  }
  if (splits.size() == 0)
        return;

  DepCompAstRef& ninfo = n->GetInfo();
  DomainInfo rest = ninfo.domain;
  for (set<DomainCond>::const_iterator sp = splits.begin();
       sp != splits.end(); ++sp) {
     DomainCond c = (*sp);
     DepCompAstRefGraphNode* n1 = CreateNode(ninfo.orig, ninfo.stmt, c);
     assert(n1 != n);
     CopySplitOperator()(this, n, n1, c);
     rest = rest - c;
     assert (!rest.IsTop());
  }
  DomainCondConstIterator conds = rest.GetConstIterator();
  DomainCond c = *conds;
  while (true) {
      conds.Advance();
      if (conds.ReachEnd())
         break; 
      DepCompAstRefGraphNode* n1 = CreateNode(ninfo.orig, ninfo.stmt, c);
      assert(n1 != n);
      CopySplitOperator()(this, n, n1, c);
      c = *conds;
  }
  ninfo.domain &= c;
  UpdateSplitOperator()(this,n, c); 
}

void DepCompAstRefGraphCreate::SplitNodeDomain() 
{
   for ( NodeIterator np = GetNodeIterator(); !np.ReachEnd(); ++np) {
          SplitNodeDomain(*np);
   }
}
*/

#define TEMPLATE_ONLY
#include <IDGraphCreate.C>
template class IDGraphCreateTemplate<GraphNodeTemplate<DepCompAstRef, DepCompAstRef const&,AstRefToString>, DepInfoEdge>;
