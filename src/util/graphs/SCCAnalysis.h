#ifndef SCC_ANALYSIS_H
#define SCC_ANALYSIS_H

#include <GraphGroup.h>

class GraphNode;
class GraphEdge;
class SCCAnalOperator 
{
  public:
    virtual void CreateSCC() = 0;
    virtual void SCCAdd( GraphNode *n) = 0;
};
 
class GraphAccess;
class SCCAnalysis
{
  public:
    void operator()( const GraphAccess *dg, SCCAnalOperator &op);
    void operator() (const GraphAccess *dg, GraphNode *node, 
                     SCCAnalOperator &op);
    void operator() (const GraphAccess *dg, GraphAccess::NodeIterator iter, 
                     SCCAnalOperator& op);
};

class SCCGroupGraphOperator : public SCCAnalOperator
{
   GroupGraphNode *sccNode;
   GroupGraphCreate *graph;
   PtrMapWrap <GraphNode, GroupGraphNode> nodeMap;
  protected:
    void SetCurSCC( GroupGraphNode *scc ) { sccNode = scc; graph->AddGroupNode(sccNode); }
  public:
   SCCGroupGraphOperator(GroupGraphCreate *g) : sccNode(0),graph(g) {}
   virtual void CreateSCC() { SetCurSCC( new GroupGraphNode(graph) ); }
   void SCCAdd( GraphNode *n)
      { sccNode->AddNode( n );
        nodeMap.InsertMapping(n, sccNode); }
   GroupGraphNode* GetSCCNode( GraphNode *n)
      { return nodeMap.Map(n); }
   GroupGraphCreate* GetGroupGraphCreate() const { return graph; }

// DQ (12/31/2005): Added virtual destructor to clean up code (eliminate compiler warning)
   virtual ~SCCGroupGraphOperator() {}
};

class DAGBaseGraphImpl;
class SCCGraphCreate : public GroupGraphCreate
{
  DAGBaseGraphImpl *impl;
 public:
  SCCGraphCreate( const GraphAccess *orig, SCCGroupGraphOperator *op = 0);
//void TopoSort( Boolean reverse = false );
  void TopoSort( int reverse = false );
};

#endif
