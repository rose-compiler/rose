#include <iostream>
#include <stdlib.h>

#include <LoopTransformInterface.h>

#include <DepGraphBuild.h>
#include <DomainInfo.h>
#include <PtrSet.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

class DepGraphEdgeCreate : public CollectObject<DepInfo>
{
 public:
   DepGraphEdgeCreate(AstTreeDepGraphBuildImpl* g,
                      GraphAccessInterface::Node *_n1, GraphAccessInterface::Node* _n2)
     :  graph(g), n1(_n1), n2(_n2) {}
   bool operator()(const DepInfo& d)
   { if (!d.IsTop()) {
        graph->CreateEdgeImpl( n1,n2,d);
        return true;
     }
     return false;
   }
 private:
  AstTreeDepGraphBuildImpl* graph;
  GraphAccessInterface::Node* n1, *n2;
};

void AstTreeDepGraphAnal ::
ComputeDataDep( const StmtNodeInfo& n1,
               const StmtNodeInfo& n2,  DepType t)
{
   DepGraphEdgeCreate deps1(graph,n1.node,n2.node),deps2(graph,n2.node,n1.node);
   impl.ComputeDataDep( n1.start, n2.start, deps1,deps2,t);
}

void AstTreeDepGraphAnal ::
ComputeStmtDep( const StmtNodeInfo& n1,
               const StmtNodeInfo& n2, int t)
{
   AstInterface& fa = LoopTransformInterface::getAstInterface();
   DepGraphEdgeCreate deps1(graph,n1.node,n2.node),deps2(graph,n2.node,n1.node);
   AstNodePtr s1 = n1.start, s2 = n2.start;
   if (DebugDep())
       std::cerr << "Computing Stmt Dep between " << AstInterface::AstToString(s1) << " and " << AstInterface::AstToString(s2) << "\n";
   if (s1 != s2 && ( (fa.IsIOInputStmt(s1) && fa.IsIOInputStmt(s2)) ||
                    (fa.IsIOOutputStmt(s1) && fa.IsIOOutputStmt(s2))) ) {
     if (t & DEPTYPE_IO)
        impl.ComputeIODep( s1, s2, deps1, deps2, DEPTYPE_IO);
   }
   else
      impl.ComputeDataDep( s1, s2, deps1,deps2,t);
}


void AstTreeDepGraphAnal::
ComputeCtrlDep( const StmtNodeInfo& nc,
                const StmtNodeInfo& ns, DepType t )
{
   AstNodePtr c = nc.start, s = ns.start;
   // bool equal  = (c == s);
   DepGraphEdgeCreate deps1(graph,nc.node, ns.node), deps2(graph, ns.node, nc.node);
   impl.ComputeCtrlDep( c, s, deps1, deps2, t);
}


bool BuildAstTreeDepGraph::
ProcessIf(AstInterface &fa, const AstNodePtr& l,
                    const AstNodePtr& cond, const AstNodePtr& truebody,
                    const AstNodePtr& falsebody, AstInterface::TraversalVisitType t)
{
  if (t == AstInterface::PreVisit) {
    GraphAccessInterface::Node *n = graph->CreateNodeImpl(l, GetStmtDomain(l));
    if (cond != AST_NULL) {
      for (StmtStackType::Iterator p(stmtNodes); !p.ReachEnd(); ++p)
         ComputeDataDep(*p, StmtNodeInfo(n,cond), DEPTYPE_DATA );
    }
    for (StmtStackType::Iterator ps(gotoNodes); !ps.ReachEnd(); ++ps) {
        StmtNodeInfo info(n,l);
        ComputeCtrlDep((*ps), info);
    }
    ctrlNodes.PushFirst(StmtNodeInfo(n,l) );
    stmtNodes.PushFirst(StmtNodeInfo(n,cond));
  }
  else {
    ctrlNodes.PopFirst();
  }
  return ProcessAstTree::ProcessIf(fa, l, cond, truebody, falsebody, t);
}

bool BuildAstTreeDepGraph::
ProcessLoop( AstInterface &fa, const AstNodePtr& l, const AstNodePtr& body,
                      AstInterface::TraversalVisitType t)
{
  if (t == AstInterface::PreVisit) {
    GraphAccessInterface::Node *n = graph->CreateNodeImpl(l, GetStmtDomain(l));
    AstNodePtr init,cond,incr;
    if (!fa.IsLoop(l, &init, &cond, &incr))
        assert(false);
    for (StmtStackType::Iterator p(stmtNodes); !p.ReachEnd(); ++p) {
      if (init != AST_NULL)
         ComputeDataDep(*p, StmtNodeInfo(n,init), DEPTYPE_DATA );
      if (cond != AST_NULL)
         ComputeDataDep(*p, StmtNodeInfo(n,cond), DEPTYPE_DATA );
      if (incr != AST_NULL)
         ComputeDataDep(*p, StmtNodeInfo(n,incr), DEPTYPE_DATA );
    }
    for (StmtStackType::Iterator ps(gotoNodes); !ps.ReachEnd(); ++ps) {
        StmtNodeInfo info(n,l);
        ComputeCtrlDep((*ps), info);
    }
    ctrlNodes.PushFirst(StmtNodeInfo(n,l) );
  }
  else {
    ctrlNodes.PopFirst();
  }
  return ProcessAstTree::ProcessLoop(fa, l, body, t);
}

bool BuildAstTreeDepGraph ::
ProcessGoto( AstInterface &fa, const AstNodePtr& s, const AstNodePtr& dest)
{
   GraphAccessInterface::Node *n = graph->CreateNodeImpl(s, GetStmtDomain(s));
   StmtNodeInfo info(n,s);
   gotoNodes.PushFirst(info);

   for (StmtStackType::Iterator ps(stmtNodes); !ps.ReachEnd(); ++ps) {
        ComputeCtrlDep( (*ps), info, DEPTYPE_BACKCTRL);
   }
   for (StmtStackType::Iterator pg(gotoNodes); !pg.ReachEnd(); ++pg) {
        ComputeCtrlDep((*pg), info);
   }
   for ( StmtStackType::Iterator p(ctrlNodes); !p.ReachEnd(); ++p) {
      ComputeCtrlDep((*p), info);
   }
   return ProcessAstTree::ProcessGoto(fa, s, dest);
}

bool BuildAstTreeDepGraph ::
ProcessStmt( AstInterface &fa, const AstNodePtr& s)
{
   GraphAccessInterface::Node *n = graph->CreateNodeImpl(s, GetStmtDomain(s));
   StmtNodeInfo info(n,s);
   stmtNodes.PushFirst(info);
   for (StmtStackType::Iterator ps(stmtNodes); !ps.ReachEnd(); ++ps) {
        ComputeStmtDep((*ps), info, DEPTYPE_DATA | DEPTYPE_IO);
   }
   for ( StmtStackType::Iterator p(ctrlNodes); !p.ReachEnd(); ++p) {
      ComputeCtrlDep((*p), info);
   }
   for (StmtStackType::Iterator pg(gotoNodes); !pg.ReachEnd(); ++pg) {
        ComputeCtrlDep((*pg), info);
   }

   return ProcessAstTree::ProcessStmt(fa, s);
}

void BuildAstTreeDepGraph :: TranslateCtrlDeps()
{
  const GraphAccessInterface* g = graph->Access();
  for (StmtStackType::Iterator p(stmtNodes); !p.ReachEnd(); ++p) {
    StmtNodeInfo &info = *p;
    GraphAccessInterface::Node *n1 = info.node;
    for (GraphAccessInterface::EdgeIterator p1 = g->GetNodeEdgeIterator(n1, GraphAccess::EdgeIn);
         !p1.ReachEnd(); ++p1) {
       GraphAccessInterface::Edge *e1 = p1.Current();
       DepInfoConstIterator depIter1 =
              graph->GetDepInfoIteratorImpl(e1, DEPTYPE_CTRL);
       if (depIter1.ReachEnd())
           continue;
       GraphAccessInterface::Node *ctrl = g->GetEdgeEndPoint(e1, GraphAccess::EdgeOut);
       DepInfo cd = depIter1.Current();
       DepInfo cd1 = Reverse( cd );
       GraphAccessInterface::EdgeIterator p2 = g->GetNodeEdgeIterator(ctrl,GraphAccess::EdgeOut);
       for ( ; !p2.ReachEnd(); ++p2) {
         GraphAccessInterface::Edge *e2 = p2.Current();
         GraphAccessInterface::Node *n2 = g->GetEdgeEndPoint(e2, GraphAccess::EdgeIn);
         for (DepInfoConstIterator depIter2 =
                             graph->GetDepInfoIteratorImpl(e2, DEPTYPE_DATA);
              ! depIter2.ReachEnd(); depIter2++) {
             DepInfo tmpd = cd1 * depIter2.Current();
             graph->CreateEdgeImpl(n1,n2,tmpd);
         }
       }
       for (p2 = g->GetNodeEdgeIterator(ctrl, GraphAccess::EdgeIn);
            !p2.ReachEnd(); ++p2) {
         GraphAccessInterface::Edge *e2 = p2.Current();
         GraphAccessInterface::Node *n2 = g->GetEdgeEndPoint(e2, GraphAccess::EdgeOut);
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


