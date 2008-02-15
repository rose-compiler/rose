
#ifndef COMPSLICE_DEPGRAPH
#define COMPSLICE_DEPGRAPH

#include <CompSlice.h>
#include <CompSliceObserver.h>
#include <DepGraph.h>
#include <PtrMap.h>
#include <SinglyLinkedList.h>

class CompSliceDepGraphNode 
   : public GraphNode, protected CompSliceNestObserver
{
  CompSliceNest vec;
  SinglyLinkedListWrap<LoopTreeNode*> rootList;
  CompSliceDepGraphNode(GraphCreate* g, LoopTreeDepComp &c, DependenceHoisting& op) ;
  CompSliceDepGraphNode(GraphCreate* g, LoopTreeDepComp &c, DependenceHoisting& op,
                        LoopTreeTransDepGraphCreate *t);

  void UpdateSwap(const CompSliceNestSwapInfo &info);
  void UpdateDeleteEntry(const CompSliceNestDeleteEntryInfo &info);
  void UpdateDuplicateEntry( const CompSliceNestDuplicateEntryInfo &info);
  void UpdateAlignEntry( const CompSliceNestAlignEntryInfo &info);
  void UpdateFusion( const CompSliceNestFusionInfo& info);
 public:
  CompSliceNest& GetInfo() { return vec; }
  std::string ToString() const { return vec.ToString(); }
  
  LoopTreeNodeIterator GetSliceRootIterator() const;
  
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
        nodeMap.InsertMapping(&node->vec, node); 
        CreateBaseNode(node);
       return node;}
  CompSliceDepGraphNode* CreateNode( LoopTreeDepComp &c, DependenceHoisting& op, 
                                     LoopTreeTransDepGraphCreate *t)
     { CompSliceDepGraphNode* node = new CompSliceDepGraphNode(this, c,op, t);
        nodeMap.InsertMapping(&node->vec, node); 
        CreateBaseNode(node); 
        return node;
     }
//Boolean DeleteNode( CompSliceDepGraphNode *n)
  int DeleteNode( CompSliceDepGraphNode *n)
      { return DepInfoGraphCreate<CompSliceDepGraphNode>::DeleteNode(n); }
  DepInfoEdge* CreateEdge( CompSliceDepGraphNode *n1, CompSliceDepGraphNode *n2,
                           const DepInfo &info);

  CompSliceDepGraphCreate(LoopTreeDepComp &comp, DependenceHoisting &op,
                          LoopTreeTransDepGraphCreate *tc=0);
  CompSliceDepGraphNode* QueryDepNode( const CompSliceNest *nest) 
      { return nodeMap.Map(const_cast<CompSliceNest*>(nest)); }

//void TopoSort( Boolean reverse = false );
  void TopoSort( int reverse = false );
};

typedef CompSliceDepGraphCreate::NodeIterator CompSliceDepGraphNodeIterator;
typedef GraphAccessTemplate<CompSliceDepGraphNode,DepInfoEdge> CompSliceDepGraph;

#endif
