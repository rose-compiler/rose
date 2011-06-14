
#ifndef BUILD_DEPGRAPH
#define BUILD_DEPGRAPH

#include <DepInfoAnal.h>
#include <DepGraph.h>
#include <ProcessAstTree.h>
#include <SinglyLinkedList.h>

class AstTreeDepGraphBuildImpl
{
 public:
  virtual GraphAccessInterface::Node* CreateNodeImpl(AstNodePtr s, const DomainCond& c) = 0;
  virtual void CreateEdgeImpl(GraphAccessInterface::Node *n1, GraphAccessInterface::Node *n2, DepInfo info) = 0;
  virtual DepInfoConstIterator 
          GetDepInfoIteratorImpl( GraphAccessInterface::Edge* e, DepType t) = 0;
  virtual AstNodePtr GetNodeAst(GraphAccessInterface::Node *n) = 0;
  virtual const GraphAccessInterface* Access() const = 0;
  virtual ~AstTreeDepGraphBuildImpl() {}
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
      GraphAccessInterface::Node *node;
      AstNodePtr start;
      StmtNodeInfo(GraphAccessInterface::Node *n, const AstNodePtr& s)
       : node(n), start(s) {}
      StmtNodeInfo() { node = 0; start = AST_NULL; }
   };

  const DomainCond& GetStmtDomain( const AstNodePtr& s) 
     { return impl.GetStmtInfo(s).domain; }

  void ComputeStmtDep( const StmtNodeInfo& n1, const StmtNodeInfo& n2, int t);
  void ComputeDataDep( const StmtNodeInfo& n1,
                      const StmtNodeInfo& n2, DepType t = DEPTYPE_ALL);
  void ComputeCtrlDep( const StmtNodeInfo& nc, const StmtNodeInfo& ns,
                       DepType t = DEPTYPE_CTRL );
};

class  BuildAstTreeDepGraph : public AstTreeDepGraphAnal, 
                              public ProcessAstTree
{
 private:
   typedef SinglyLinkedListWrap <StmtNodeInfo> StmtStackType;
   StmtStackType stmtNodes, ctrlNodes, gotoNodes, inputNodes, outputNodes;
 public:
  BuildAstTreeDepGraph( AstTreeDepGraphBuildImpl* g, DepInfoAnal &_impl)
    : AstTreeDepGraphAnal(g, _impl) {}

  bool ProcessStmt(AstInterface &fa, const AstNodePtr& s);
  bool ProcessGoto(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& dest);
  bool ProcessIf(AstInterface &fa, const AstNodePtr& l, 
                    const AstNodePtr& cond, const AstNodePtr& truebody,
                    const AstNodePtr& falsebody, AstInterface::TraversalVisitType t)
     { if (t == AstInterface::PreVisit) return ProcessStmt(fa, l); 
       return true;
     }
  bool ProcessLoop(AstInterface &fa, const AstNodePtr& l, const AstNodePtr& body,
                      AstInterface::TraversalVisitType t);
  GraphAccessInterface::Node* LastProcessedStmt() 
    { return stmtNodes.First()->GetEntry().node; }
  void TranslateCtrlDeps();
};

#endif

