
#include <general.h>
#include <iostream>
#include <stdlib.h>

#include <LoopTransformInterface.h>

#include <DepGraphBuild.h>
#include <DomainInfo.h>
#include <PtrSet.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

class DepGraphEdgeCreate : public CollectObject<DepInfo>
{
  AstTreeDepGraphBuildImpl* graph;
  GraphNode* n1, *n2;   
 public:
   DepGraphEdgeCreate(AstTreeDepGraphBuildImpl* g, 
                      GraphNode *_n1, GraphNode* _n2)
     : graph(g), n1(_n1), n2(_n2) {}
   Boolean operator()(const DepInfo& d) 
   { if (!d.IsTop()) {
        graph->CreateEdgeImpl( n1,n2,d);
        return true;
     }
     return false;
   }
};

void AstTreeDepGraphAnal :: 
ComputeDataDep(LoopTransformInterface &fa,const StmtNodeInfo& n1,
               const StmtNodeInfo& n2,  DepType t)
{
   DepGraphEdgeCreate deps1(graph,n1.node,n2.node),deps2(graph,n2.node,n1.node);
   impl.ComputeDataDep( fa,n1.start, n2.start, deps1,deps2,t);
}

void AstTreeDepGraphAnal ::
ComputeStmtDep(LoopTransformInterface &la,const StmtNodeInfo& n1,
               const StmtNodeInfo& n2, int t)
{
   AstInterface& fa = la;
   DepGraphEdgeCreate deps1(graph,n1.node,n2.node),deps2(graph,n2.node,n1.node);
   AstNodePtr s1 = n1.start, s2 = n2.start;
   if (s1 != s2 && ( (fa.IsIOInputStmt(s1) && fa.IsIOInputStmt(s2)) ||
                    (fa.IsIOOutputStmt(s1) && fa.IsIOOutputStmt(s2))) ) {
     if (t & DEPTYPE_IO)
        impl.ComputeIODep( la, s1, s2, deps1, deps2, DEPTYPE_IO);
   }
   else 
      impl.ComputeDataDep( la,s1, s2, deps1,deps2,t);
}


void AstTreeDepGraphAnal::
ComputeCtrlDep( LoopTransformInterface &fa,const StmtNodeInfo& nc,
                const StmtNodeInfo& ns, DepType t )
{
   AstNodePtr c = nc.start, s = ns.start;
   // bool equal  = (c == s);
   DepGraphEdgeCreate deps1(graph,nc.node, ns.node), deps2(graph, ns.node, nc.node);
   impl.ComputeCtrlDep( fa, c, s, deps1, deps2, t);
}


Boolean BuildAstTreeDepGraph::
ProcessLoop( AstInterface &fa, const AstNodePtr& l, const AstNodePtr& body,
                      AstInterface::TraversalVisitType t)
{
  if (t == AstInterface::PreVisit) {
    GraphNode *n = graph->CreateNodeImpl(l, GetStmtDomain(lf, l));
    AstNodePtr init,cond,incr;
    if (!fa.IsLoop(l, &init, &cond, &incr))
        assert(false);
    for (StmtStackType::Iterator p(stmtNodes); !p.ReachEnd(); ++p) {
      if (init != 0)
         ComputeDataDep(lf, *p, StmtNodeInfo(n,init), DEPTYPE_DATA );
      if (cond != 0)
         ComputeDataDep(lf, *p, StmtNodeInfo(n,cond), DEPTYPE_DATA );
      if (incr != 0)
         ComputeDataDep(lf, *p, StmtNodeInfo(n,incr), DEPTYPE_DATA );
    }
    for (StmtStackType::Iterator ps(gotoNodes); !ps.ReachEnd(); ++ps) {
        StmtNodeInfo info(n,l);
        ComputeCtrlDep(lf, (*ps), info);
    }
    ctrlNodes.PushFirst(StmtNodeInfo(n,l) );
  }
  else {
    ctrlNodes.PopFirst();
  }
  return ProcessAstTree::ProcessLoop(fa, l, body, t);
}

Boolean BuildAstTreeDepGraph ::
ProcessGoto( AstInterface &fa, const AstNodePtr& s, const AstNodePtr& dest)
{
   GraphNode *n = graph->CreateNodeImpl(s, GetStmtDomain(lf, s));
   StmtNodeInfo info(n,s);
   gotoNodes.PushFirst(info);

   for (StmtStackType::Iterator ps(stmtNodes); !ps.ReachEnd(); ++ps) {
        ComputeCtrlDep( lf, (*ps), info, DEPTYPE_BACKCTRL);
   }
   for (StmtStackType::Iterator pg(gotoNodes); !pg.ReachEnd(); ++pg) {
        ComputeCtrlDep(lf, (*pg), info);
   }
   for ( StmtStackType::Iterator p(ctrlNodes); !p.ReachEnd(); ++p) {
      ComputeCtrlDep(lf, (*p), info);
   }
   return ProcessAstTree::ProcessGoto(fa, s, dest);
}

void BuildAstTreeDepGraph ::
ProcessStmt( AstInterface &fa, const AstNodePtr& s)
{
   GraphNode *n = graph->CreateNodeImpl(s, GetStmtDomain(lf, s));
   StmtNodeInfo info(n,s);
   stmtNodes.PushFirst(info);
   for (StmtStackType::Iterator ps(stmtNodes); !ps.ReachEnd(); ++ps) {
        ComputeStmtDep(lf, (*ps), info, DEPTYPE_DATA | DEPTYPE_IO);
   }
   for ( StmtStackType::Iterator p(ctrlNodes); !p.ReachEnd(); ++p) {
      ComputeCtrlDep(lf, (*p), info);
   }
   for (StmtStackType::Iterator pg(gotoNodes); !pg.ReachEnd(); ++pg) {
        ComputeCtrlDep(lf, (*pg), info);
   }

   ProcessAstTree::ProcessStmt(fa, s);
}

void BuildAstTreeDepGraph :: TranslateCtrlDeps(LoopTransformInterface &fa)
{
  GraphAccess* g = graph->Access();
  for (StmtStackType::Iterator p(stmtNodes); !p.ReachEnd(); ++p) {
    StmtNodeInfo &info = *p;
    GraphNode *n1 = info.node;
    for (GraphAccess::EdgeIterator p1 = g->GetNodeEdgeIterator(n1, GraphAccess::EdgeIn);
         !p1.ReachEnd(); ++p1) {
       GraphEdge *e1 = p1.Current();
       DepInfoConstIterator depIter1 = 
              graph->GetDepInfoIteratorImpl(e1, DEPTYPE_CTRL);
       if (depIter1.ReachEnd())
           continue;
       GraphNode *ctrl = g->GetEdgeEndPoint(e1, GraphAccess::EdgeOut);
       DepInfo cd = depIter1.Current();
       DepInfo cd1 = Reverse( cd );
       GraphAccess::EdgeIterator p2 = g->GetNodeEdgeIterator(ctrl,GraphAccess::EdgeOut);
       for ( ; !p2.ReachEnd(); ++p2) {
         GraphEdge *e2 = p2.Current();
         GraphNode *n2 = g->GetEdgeEndPoint(e2, GraphAccess::EdgeIn);
         for (DepInfoConstIterator depIter2 = 
                             graph->GetDepInfoIteratorImpl(e2, DEPTYPE_DATA);
              ! depIter2.ReachEnd(); depIter2++) {
             DepInfo tmpd = cd1 * depIter2.Current();
             graph->CreateEdgeImpl(n1,n2,tmpd);
         }
       }
       for (p2 = g->GetNodeEdgeIterator(ctrl, GraphAccess::EdgeIn);
            !p2.ReachEnd(); ++p2) {
         GraphEdge *e2 = p2.Current();
         GraphNode *n2 = g->GetEdgeEndPoint(e2, GraphAccess::EdgeOut);
         for (DepInfoConstIterator depIter2 = 
                     graph->GetDepInfoIteratorImpl(e2, DEPTYPE_DATA);
              ! depIter2.ReachEnd(); depIter2++)  {
             DepInfo tmpd = depIter2.Current() * cd;
             graph->CreateEdgeImpl(n2,n1, tmpd);
         }
       }
    }
  }
}


