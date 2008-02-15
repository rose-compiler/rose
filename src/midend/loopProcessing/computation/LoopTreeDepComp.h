
#ifndef LOOPTREE_DEPCOMP
#define LOOPTREE_DEPCOMP

#include <DepGraph.h>
#include <TransDepGraph.h>
#include <DepInfoAnal.h>
#include <DomainInfo.h>
#include <DepRel.h>
#include <LoopTree.h>
#include <LoopTreeObserver.h>
#include <PtrMap.h>
#include <PtrSet.h>
#include <LoopTransformInterface.h>
#include <GraphScope.h>

class LoopTreeToString{
 public:
  static std::string ToString( const LoopTreeNode* n) { return n->ToString(); }
};

class LoopTreeDepGraphNode 
   : public GraphObserveNodeTemplate<LoopTreeNode*, LoopTreeNode*, LoopTreeToString>,
     public LoopTreeObserver
{
   DepInfo loopMap;   
   DomainCond domain;
   ~LoopTreeDepGraphNode() 
     { 
      LoopTreeNode *s = GetInfo();
      if (s && s->GetOrigStmt() != 0)
        s->DetachObserver(*this);
     }
  void UpdateDeleteNode( const LoopTreeNode *n) { GetInfo() = 0; }
  void UpdateSwapStmtLoop( const SwapStmtLoopInfo &info)
       { loopMap.SwapLoop(info.GetLoop1(),info.GetLoop2(),DEP_SINK); 
         domain.SwapLoop(info.GetLoop1(), info.GetLoop2());
       }
  void UpdateMergeStmtLoop( const MergeStmtLoopInfo &info)
       { loopMap.MergeLoop(info.GetLoop1(), info.GetLoop2(),DEP_SINK); 
         domain.MergeLoop(info.GetLoop1(), info.GetLoop2());
         if (info.GetMergeAlign() != 0) {
           loopMap.AlignLoop(info.GetLoop1(), -info.GetMergeAlign(),DEP_SINK);
           domain.AlignLoop(info.GetLoop1(), -info.GetMergeAlign());
         }
       }
   LoopTreeDepGraphNode(GraphCreate *c, LoopTreeNode *s = 0)
       : GraphObserveNodeTemplate<LoopTreeNode*,LoopTreeNode*,LoopTreeToString>(c,s) 
       { if (s != 0) {
            int level = s->LoopLevel();
            loopMap = DepInfoGenerator::GetIDDepInfo(level,false);
            domain = DomainCond(level);
         }    
       } 
   LoopTreeDepGraphNode(GraphCreate *c, LoopTreeNode* s,
                        const DomainCond _dm)
     : GraphObserveNodeTemplate<LoopTreeNode*,LoopTreeNode*,LoopTreeToString>(c, s),
       loopMap(DepInfoGenerator::GetIDDepInfo(s->LoopLevel(),false)), 
       domain(_dm)
     { if (s->GetOrigStmt() != 0)
         s->AttachObserver(*this);
     }
   LoopTreeDepGraphNode(GraphCreate *c, LoopTreeNode* s,
                        const DepInfo& _loopMap, const DomainCond _dm)
     : GraphObserveNodeTemplate<LoopTreeNode*,LoopTreeNode*,LoopTreeToString>(c, s),
       loopMap(_loopMap), domain(_dm)
     { if (s->GetOrigStmt() != 0) 
	 s->AttachObserver(*this);
     }

   void UpdateInsertStmtLoop( const InsertStmtLoopInfo& info)
       { 
         int level = info.GetLoop();
         loopMap.InsertLoop(level, DEP_SINK); 
         domain.InsertLoop(level);
       }
  void UpdateDeleteStmtLoop( const DeleteStmtLoopInfo& info)
       { 
         int level = info.GetLoop();
         loopMap.RemoveLoop(level, DEP_SINK); 
         domain.RemoveLoop(level);
       }
 public:
   int NumOfLoops() const { return loopMap.cols(); }
   int AstTreeNumOfLoops() const { return loopMap.rows(); }
   int AstTreeDim2LoopTreeDim( int index) 
      { for (int i = 0; i < loopMap.cols(); ++i)
             if (!loopMap.Entry(index,i).IsBottom()) return i;
        assert(false); }
   int LoopTreeDim2AstTreeDim( int index)
      { for (int i = 0; i < loopMap.rows(); ++i)
             if (!loopMap.Entry(i,index).IsBottom()) return i;
        assert(false); }
   const DepInfo& GetLoopMap() const { return loopMap; }
   const DomainCond& GetDomain() const { return domain; }

 friend class LoopTreeDepGraphCreate;
};

typedef GraphAccessTemplate<LoopTreeDepGraphNode,DepInfoEdge> LoopTreeDepGraph;
typedef LoopTreeDepGraph::NodeIterator LoopTreeDepGraphNodeIterator;
typedef TransDepGraphCreate<LoopTreeDepGraphNode> LoopTreeTransDepGraphCreate;

class LoopTreeNodeDepMap
{
  PtrMapWrap <LoopTreeNode, LoopTreeDepGraphNode>* map;
  void InsertMapping( const LoopTreeNode*  info, LoopTreeDepGraphNode *n)
        { map->InsertMapping(info, n); } 
 public:
  LoopTreeNodeDepMap(PtrMapWrap <LoopTreeNode, LoopTreeDepGraphNode>* m) : map(m) {}
  LoopTreeNodeDepMap( const LoopTreeNodeDepMap& that) : map(that.map) {}
  LoopTreeNodeDepMap() { map = 0; }

  LoopTreeDepGraphNode * GetDepNode( const LoopTreeNode* s) const
     {  return map->Map(s); }
  LoopTreeNode * GetTreeNode(const LoopTreeDepGraphNode * n) const
          { return n->GetInfo(); }
 friend class LoopTreeDepGraphCreate;
};

class LoopTreeDepGraphCreate 
   : public DepInfoGraphCreate<LoopTreeDepGraphNode>, 
     public LoopTreeObserver
{
  LoopTreeNodeDepMap map;

  void UpdateDeleteNode( const LoopTreeNode *n);
  void UpdateSplitStmt( const SplitStmtInfo &info);
  void UpdateSplitStmt2( const SplitStmtInfo2 &info);
  void UpdateSwapStmtLoop( const SwapStmtLoopInfo &info);
  void UpdateMergeStmtLoop( const MergeStmtLoopInfo &info);
  void UpdateDeleteStmtLoop( const DeleteStmtLoopInfo &info);
  void UpdateDistNode(const DistNodeInfo &info);
  void UpdateInsertLoop( const InsertLoopInfo &info);
 public:
  LoopTreeDepGraphCreate( const LoopTreeNodeDepMap& m, 
			  BaseGraphCreate *b = 0 )
    : DepInfoGraphCreate<LoopTreeDepGraphNode>(b), map(m) {}
  ~LoopTreeDepGraphCreate();
  
  void BuildDep( LoopTransformInterface &fa, DepInfoAnal &anal, 
                LoopTreeDepGraphNode *n1, LoopTreeDepGraphNode *n2, 
		DepType t);

  void AddNode(LoopTreeDepGraphNode* result);
  LoopTreeDepGraphNode* CreateNode(LoopTreeNode *s, LoopTreeDepGraphNode* that = 0);
  LoopTreeDepGraphNode* CreateNode(LoopTreeNode *s, const DomainCond& c);
  LoopTreeDepGraphNode* CreateNode(LoopTreeNode *s, const DepInfo& m,
                                    const DomainCond& c);
//Boolean DeleteNode( LoopTreeDepGraphNode *n);
  int DeleteNode( LoopTreeDepGraphNode *n);
  DepInfoEdge* CreateEdge( LoopTreeDepGraphNode *n1, LoopTreeDepGraphNode *n2,
			   const DepInfo& info);
  DepInfoEdge* CreateEdgeFromOrigAst( LoopTreeDepGraphNode *n1,
                                      LoopTreeDepGraphNode *n2, const DepInfo& info);

  typedef GraphSelectEndSet2<LoopTreeDepGraph,
                             SelectPtrSet<const LoopTreeDepGraphNode> >
           SelectNode;
  typedef GraphSelectCompound<LoopTreeDepGraph, SelectNode,SelectDepLevel> 
          SelectSubtree;
  typedef GraphSelect<LoopTreeDepGraph, 
             GraphSelectCompound<LoopTreeDepGraph, SelectNode,SelectDepLevel> >
          SubtreeGraph;
  static SubtreeGraph GetSubtreeGraph(LoopTreeDepGraph* g, const LoopTreeNodeDepMap& map, LoopTreeNode* r);
};


class LoopTreeDepComp 
{
  LoopTreeNode *root;
  LoopTreeCreate* tc;
  LoopTreeDepGraph *depGraph;
  LoopTreeNodeDepMap map;
 protected:
  LoopTreeDepComp() { root = 0; depGraph = 0; tc = 0; }
  LoopTreeDepComp(LoopTreeDepComp& that) 
    { map = that.map; root = that.root;  tc = that.tc;
     depGraph = that.depGraph; }
  void SetTreeRoot(LoopTreeNode *r) { root = r; }
  void SetDepGraph( LoopTreeDepGraph *d) { depGraph = d; }
  void SetMap(const LoopTreeNodeDepMap& _map) { map = _map; }
  void SetTreeCreate( LoopTreeCreate* c) { tc = c; }
 public:
  LoopTreeCreate* GetLoopTreeCreate() { return tc; }

  LoopTreeNode * GetLoopTreeRoot() const { return root; }
  LoopTreeDepGraph * GetDepGraph() const { return depGraph; }

  LoopTreeDepGraphNode * GetDepNode( const LoopTreeNode* s) const
     { return map.GetDepNode(s); }
  LoopTreeNode * GetTreeNode(const LoopTreeDepGraphNode * n) const
          { return n->GetInfo(); }
  LoopTreeNodeDepMap GetTreeNodeMap() const { return map; }
  const DomainCond& GetDomain(LoopTreeNode* s)
       { return GetDepNode(s)->GetDomain(); }

  void Dump() const;
  void DumpTree() const;
  void DumpDep() const;
  void DumpNode( LoopTreeNode *s) const;
};

class LoopTreeDepCompSubtree : public LoopTreeDepComp
{
  typedef GraphScopeTemplate<LoopTreeDepGraphNode,DepInfoEdge,
                      LoopTreeDepGraphCreate::SubtreeGraph> ScopeType;
  ScopeType scope;
 public:
  LoopTreeDepCompSubtree( LoopTreeDepComp &c, LoopTreeNode* root);
  LoopTreeDepCompSubtree( LoopTreeDepCompSubtree& that) 
     : LoopTreeDepComp(that), scope(that.scope) {}
};

class LoopTreeDepCompCreate : public LoopTreeDepComp, public GraphObserver
{
  DepInfoAnal anal;
  PtrMapWrap <LoopTreeNode, LoopTreeDepGraphNode> nodeMap;
  LoopTreeDepGraphCreate* depCreate;
  LoopTreeCreate treeCreate;
  AstNodePtr top;

  void UpdateDeleteNode( const GraphNode *n);
 public:
  LoopTreeDepCompCreate( LoopTransformInterface &fa, const AstNodePtr& top, 
                         bool buildDepGraph = true);
  ~LoopTreeDepCompCreate(); 

  void BuildDepGraph( LoopTransformInterface &la);
  void DetachDepGraph();

  DepInfoAnal& GetDepAnal() { return anal; }
  AstNodePtr CodeGen( LoopTransformInterface &fa);

};

#endif
