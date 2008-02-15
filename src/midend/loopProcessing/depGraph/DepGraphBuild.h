
#ifndef BUILD_DEPGRAPH
#define BUILD_DEPGRAPH

#include <DepInfoAnal.h>
#include <DepGraph.h>
#include <ProcessAstTree.h>
#include <SinglyLinkedList.h>

class AstTreeDepGraphBuildImpl
{
 public:
  virtual GraphNode* CreateNodeImpl(AstNodePtr s, const DomainCond& c) = 0;
  virtual void CreateEdgeImpl(GraphNode *n1, GraphNode *n2, DepInfo info) = 0;
  virtual DepInfoConstIterator 
          GetDepInfoIteratorImpl( GraphEdge* e, DepType t) = 0;
  virtual AstNodePtr GetNodeAst(GraphNode *n) = 0;
  virtual GraphAccess* Access() const = 0;
};

class  AstTreeDepGraphAnal
{
  DepInfoAnal &impl;
 protected:
  AstTreeDepGraphBuildImpl *graph;
 public:
   AstTreeDepGraphAnal( AstTreeDepGraphBuildImpl* g, DepInfoAnal &_impl) 
      : impl(_impl), graph(g) {}
   struct StmtNodeInfo {
      GraphNode *node;
      AstNodePtr start;
      StmtNodeInfo(GraphNode *n, const AstNodePtr& s)
       : node(n), start(s) {}
      StmtNodeInfo() { node = 0; start = 0; }
   };

  const DomainCond& GetStmtDomain( LoopTransformInterface &fa, const AstNodePtr& s) 
     { return impl.GetStmtInfo(fa, s).domain; }

  void ComputeStmtDep(LoopTransformInterface &fa,const StmtNodeInfo& n1, const StmtNodeInfo& n2, int t);
  void ComputeDataDep(LoopTransformInterface &fa, const StmtNodeInfo& n1,
                      const StmtNodeInfo& n2, DepType t = DEPTYPE_ALL);
  void ComputeCtrlDep( LoopTransformInterface &fa,const StmtNodeInfo& nc, const StmtNodeInfo& ns,
                       DepType t = DEPTYPE_CTRL );
};

class  BuildAstTreeDepGraph : public AstTreeDepGraphAnal, 
                              public ProcessAstTree
{
 private:
   typedef SinglyLinkedListWrap <StmtNodeInfo> StmtStackType;
   StmtStackType stmtNodes, ctrlNodes, gotoNodes, inputNodes, outputNodes;
   LoopTransformInterface &lf;
 public:
  BuildAstTreeDepGraph( LoopTransformInterface& _lf, AstTreeDepGraphBuildImpl* g, DepInfoAnal &_impl)
    : AstTreeDepGraphAnal(g, _impl), lf(_lf) {}

//Boolean ProcessStmt(AstInterface &fa, const AstNodePtr& s);
  void ProcessStmt(AstInterface &fa, const AstNodePtr& s);
//Boolean ProcessGoto(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& dest);
  int ProcessGoto(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& dest);
//Boolean ProcessIf(AstInterface &fa, const AstNodePtr& l, 
  void ProcessIf(AstInterface &fa, const AstNodePtr& l, 
                    const AstNodePtr& cond, const AstNodePtr& truebody,
                    const AstNodePtr& falsebody, AstInterface::TraversalVisitType t)
     { if (t == AstInterface::PreVisit) 
	 ProcessStmt(fa, l); 
     }
//Boolean ProcessLoop(AstInterface &fa, const AstNodePtr& l, const AstNodePtr& body,
  int ProcessLoop(AstInterface &fa, const AstNodePtr& l, const AstNodePtr& body,
                      AstInterface::TraversalVisitType t);
  GraphNode* LastProcessedStmt() 
    { return stmtNodes.First()->GetEntry().node; }
  void TranslateCtrlDeps(LoopTransformInterface &fa);
};

#endif

