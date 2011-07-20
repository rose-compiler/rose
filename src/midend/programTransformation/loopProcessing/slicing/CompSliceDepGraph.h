
#ifndef COMPSLICE_DEPGRAPH
#define COMPSLICE_DEPGRAPH

#include <CompSlice.h>
#include <CompSliceObserver.h>
#include <DepGraph.h>
#include <PtrMap.h>
#include <SinglyLinkedList.h>

class CompSliceDepGraphNode 
   : public MultiGraphElem, protected CompSliceNestObserver
{
 public:
  class FullNestInfo;
  class NestInfo {
     NestInfo* innerNest;
  public:
     NestInfo() : innerNest(0) {}
     virtual ~NestInfo() {}
     virtual CompSliceNest* GetNest() { return 0; }
     virtual const CompSliceNest* GetNest() const { return 0; }
     NestInfo* InnerNest() { return innerNest; }
     const NestInfo* InnerNest() const { return innerNest; }
     /*QY: return the root to apply slicing transformation within parent;
          to be correct, must be invoked in topo-order of DepGraph*/
     virtual LoopTreeNode* GenXformRoot(LoopTreeNode* parent) const = 0;
     virtual std::string toString() const = 0;
   friend class CompSliceDepGraphNode;
  };
  class FullNestInfo : public NestInfo{
     CompSliceNest vec;
     FullNestInfo(CompSlice* slice,int maxsize) : vec(maxsize)
        { vec.Append(slice); }
     FullNestInfo() {}
  public:
     CompSliceNest* GetNest() { return &vec; }
     const CompSliceNest* GetNest() const { return &vec; }
     virtual LoopTreeNode* GenXformRoot(LoopTreeNode* parent) const;
     virtual std::string toString() const { return vec.toString() ; }
   friend class CompSliceDepGraphNode;
  };
  class EmptyNestInfo : public NestInfo 
  {
    std::set<LoopTreeNode*> nodes;
     EmptyNestInfo() {}
     EmptyNestInfo(LoopTreeNode* r) { nodes.insert(r); }
     EmptyNestInfo(const CompSlice* slice) 
      { 
          assert(slice != 0); 
          for (CompSlice::ConstStmtIterator stmts = slice->GetConstStmtIterator(); 
              !stmts.ReachEnd(); ++stmts)
                nodes.insert(stmts.Current());
     }
  public:
     LoopTreeNode* GenXformRoot(LoopTreeNode* parent) const;
     virtual std::string toString() const { 
         std::string res ="EMPTY slice: ";
         for (std::set<LoopTreeNode*>::const_iterator p = nodes.begin();
              p != nodes.end(); ++p) {
            res = res + (*p)->toString();
         }
         return res;
     }
   friend class CompSliceDepGraphNode;
  };

 private:
  NestInfo* impl;
  CompSliceDepGraphNode(MultiGraphCreate* g, LoopTreeDepComp &c, DependenceHoisting& op) ;
  CompSliceDepGraphNode(MultiGraphCreate* g, LoopTreeDepComp &c, DependenceHoisting& op,
                        LoopTreeTransDepGraphCreate *t);

  void UpdateSwap(const CompSliceNestSwapInfo &info);
  void UpdateDeleteEntry(const CompSliceNestDeleteEntryInfo &info);
  void UpdateDuplicateEntry( const CompSliceNestDuplicateEntryInfo &info);
  void UpdateAlignEntry( const CompSliceNestAlignEntryInfo &info);
  void UpdateFusion( const CompSliceNestFusionInfo& info);
 public:
  ~CompSliceDepGraphNode() { delete impl; }
  std::string toString() const { return impl->toString(); }
  NestInfo& GetInfo() { assert(impl!=0); return *impl; }
  LoopTreeNodeIterator GetStmtIterator() const;
 friend class CompSliceDepGraphCreate;
};

class DAGBaseGraphImpl;
class CompSliceDepGraphCreate 
   : public DepInfoGraphCreate<CompSliceDepGraphNode>
{
  DAGBaseGraphImpl *impl;
  PtrMapWrap <CompSliceNest, CompSliceDepGraphNode> nodeMap;
 public:

  void MoveEdgeEndPoint(DepInfoEdge *e, EdgeDirection dir, 
                        CompSliceDepGraphNode *n);
  CompSliceDepGraphNode* CreateNode(LoopTreeDepComp &c, DependenceHoisting& op)
     { CompSliceDepGraphNode* node = new CompSliceDepGraphNode(this, c, op);
        nodeMap.InsertMapping(node->GetInfo().GetNest(), node); 
        AddNode(node);
       return node;}
  CompSliceDepGraphNode* CreateNode( LoopTreeDepComp &c, DependenceHoisting& op, 
                                     LoopTreeTransDepGraphCreate *t)
     { CompSliceDepGraphNode* node = new CompSliceDepGraphNode(this, c,op, t);
        nodeMap.InsertMapping(node->GetInfo().GetNest(), node); 
        AddNode(node); 
        return node;
     }
  bool DeleteNode( CompSliceDepGraphNode *n)
      { return DepInfoGraphCreate<CompSliceDepGraphNode>::DeleteNode(n); }
  DepInfoEdge* CreateEdge( CompSliceDepGraphNode *n1, CompSliceDepGraphNode *n2,
                           const DepInfo &info);

  CompSliceDepGraphCreate(LoopTreeDepComp &comp, DependenceHoisting &op,
                          LoopTreeTransDepGraphCreate *tc=0);
  CompSliceDepGraphNode* QueryDepNode( const CompSliceNest *nest) 
      { return nodeMap.Map(const_cast<CompSliceNest*>(nest)); }

  void TopoSort( bool reverse = false );
};

typedef CompSliceDepGraphCreate::NodeIterator CompSliceDepGraphNodeIterator;
typedef GraphAccessTemplate<CompSliceDepGraphNode,DepInfoEdge> CompSliceDepGraph;

#endif
