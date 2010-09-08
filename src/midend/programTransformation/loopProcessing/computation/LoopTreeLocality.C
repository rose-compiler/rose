
#include <stdlib.h>
#include <sstream>

#include <LoopTreeLocality.h>
#include <ReuseAnalysis.h>
#include <DepGraphTransform.h>
#include <CommandOptions.h>
#include <StmtInfoCollect.h>
#include <StmtDepAnal.h>

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
ComputeInputDep( LoopTreeDepGraph::NodeIterator nodeIter, DepCompAstRefAnal& stmtorder)
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
  GraphCrossEdgeIterator<LoopTreeDepGraph> p(&inputCreate,n1,n2);
  if (!p.ReachEnd())
    return;
  
  inputCreate.BuildDep(fa, anal, n1,n2,DEPTYPE_INPUT);
}

void LoopTreeLocalityAnal ::
ComputeInputDep( LoopTreeDepGraph::NodeIterator srcIter,
                 LoopTreeDepGraph::NodeIterator snkIter, DepCompAstRefAnal& stmtorder)
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
  std::string ivarname;
  int linesize;
 public:
  bool operator()(const AstNodePtr& cur)
  { res +=  SelfSpatialReuse( la, cur, ivarname, linesize); return true;}
  AccumulateSpatialReuse( LoopTransformInterface& _la, const std::string& _ivarname, int _linesize)
    : res(0), la(_la), ivarname(_ivarname), linesize(_linesize) {}
  float get_result() const { return res; }
};

float LoopTreeLocalityAnal ::
SelfSpatialReuses( LoopTreeNode *n, int loop, int linesize)
{
   int loop1 = comp.GetDepNode(n)->LoopTreeDim2AstTreeDim(loop);
   AstNodePtr s = n->GetOrigStmt();
   std::string name = anal.GetStmtInfo(fa, s).ivars[loop1].GetVarName();
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
  GraphCrossEdgeIterator<LoopTreeDepGraph> iter1 (graph,n1,n2); 
  GraphCrossEdgeIterator<LoopTreeDepGraph> iter2 (&inputCreate,n1,n2);
  MultiIterator<DepInfoEdge*, GraphCrossEdgeIterator<LoopTreeDepGraph> > 
          iter3(iter1,iter2);
  DepInfoEdgeConstInfoIterator<
    MultiIterator<DepInfoEdge*, 
                  GraphCrossEdgeIterator<LoopTreeDepGraph> > > iter4(iter3);
  MapSrcSinkLooplevel loopmap(loop1, loop2);

  AppendSTLSet<AstNodePtr> col(refSet);
  ::TemporaryReuseRefs( iter4, loopmap, col, &reuseDist);
  return refSet.size();
}

std::string DepCompAstRef::ToHandle() const 
{
   std::stringstream res;
   res <<  stmt  << orig.get_ptr();
   return res.str();
}

std::string DepCompAstRef::toString() const 
{
  std::stringstream out;
  out << AstToString(orig) << " : " << stmt << ":" << stmt->toString(); 
  return out.str();
}

DepCompAstRefGraphNode* DepCompAstRefGraphCreate::
CreateNode(const AstNodePtr& r, LoopTreeNode* stmt)
{
    DepCompAstRef cur(r, stmt);
    AstRefNodeMap::const_iterator rp = refNodeMap.find(cur);

    DepCompAstRefGraphNode* rnode = 0;
    if (rp == refNodeMap.end()) {
       rnode = new DepCompAstRefGraphNode(this, cur);
       AddNode(rnode);
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
   AddEdge(src, snk, e);
   if (DebugRefGraph()) {
      std::cerr << "creating edge in ref graph: " << src->toString() << "->" << snk->toString() << " : " << e->toString() << std::endl;
   }
   return e;
}

template<class Graph, class EdgeIterator>
void CreateEdgeWrap( LoopTransformInterface& la, DepCompAstRefGraphCreate* res, 
            LoopTreeDepComp& comp, Graph* g, EdgeIterator deps)
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
  LoopTreeDepGraphSubtree depgraph(comp, root, comp.GetDepGraph());
  GraphEdgeIterator<LoopTreeDepGraphSubtree> edgep(&depgraph);
  CreateEdgeWrap( la, this, comp, &depgraph, edgep); 

  LoopTreeDepGraphSubtree inputgraph(comp,root, tc.GetInputGraph());
  GraphEdgeIterator<LoopTreeDepGraphSubtree> edgep1(&inputgraph);
  CreateEdgeWrap( la, this, comp, &inputgraph, edgep1);
}
     
void DepCompAstRefAnal:: Append(LoopTransformInterface& ai, LoopTreeNode* _root)
     {
       
       typedef std::map<AstNodePtr,int,std::less<AstNodePtr> > AstIntMap;
       class CollectModRef : public CollectObject<std::pair<AstNodePtr,AstNodePtr> >  
        { 
         AstIntMap& refmap;
         public:
         CollectModRef(AstIntMap&  m) : refmap(m){}
         bool operator()(const std::pair<AstNodePtr,AstNodePtr>& cur)
          {
            if (refmap.find(cur.first) == refmap.end()) {
                int num = refmap.size();
                refmap[cur.first] = -num;
                return true;
            }
            return false;
          }
       } modcollect(refmap);
       class CollectReadRef : public CollectObject<std::pair<AstNodePtr,AstNodePtr> >  
        { 
         AstIntMap& refmap;
         public:
         CollectReadRef(AstIntMap&  m) : refmap(m){}
         bool operator()(const std::pair<AstNodePtr,AstNodePtr>& cur)
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
       for (LoopTreeTraverse stmts(_root, LoopTreeTraverse::PostOrder); 
            !stmts.ReachEnd(); stmts.Advance(),++stmtnum) {
           LoopTreeNode* curstmt = stmts.Current();
           assert(stmtmap.find(curstmt) == stmtmap.end());
           stmtmap[curstmt] = stmtnum; 
           StmtSideEffectCollect(ai.getSideEffectInterface())(ai, curstmt->GetOrigStmt(), &modcollect, &readcollect);  
       } 
     }

bool DepCompAstRefGraphCreate::
SelfReuseLevel( const DepCompAstRefGraphNode* n, int level) const
{
  GraphCrossEdgeIterator<DepCompAstRefGraphCreate> edges(this, n,n);
  for ( ; !edges.ReachEnd(); ++edges) {
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

