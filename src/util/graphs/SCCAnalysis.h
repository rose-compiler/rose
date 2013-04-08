#ifndef SCC_ANALYSIS_H
#define SCC_ANALYSIS_H

#include <GraphAccess.h>
#include <GraphGroup.h>
#include "rosedll.h"

class SCCAnalOperator 
{
  public:
    virtual void CreateSCC() = 0;
    virtual void SCCAdd( GraphAccessInterface::Node *n) = 0;
    virtual ~SCCAnalOperator() {}
};
 
class SCCAnalysis
{
  public:
    void operator()( const GraphAccessInterface *dg, SCCAnalOperator &op);
    void operator() (const GraphAccessInterface *dg, 
                     GraphAccessInterface::Node *node, 
                     SCCAnalOperator &op);
    void operator() (const GraphAccessInterface *dg, 
                     GraphAccessInterface::NodeIterator iter, 
                     SCCAnalOperator& op);
};

class SCCGroupGraphOperator : public SCCAnalOperator
{
  public:
   SCCGroupGraphOperator(GroupGraphCreate *g) : sccNode(0), graph(g) {}
   virtual ~SCCGroupGraphOperator() {}
   virtual void CreateSCC() { SetCurSCC( new GroupGraphNode(graph) ); }
   void SCCAdd( GraphAccessInterface::Node *n)
      { sccNode->AddNode( n );
        nodeMap.InsertMapping(n, sccNode); }
   GroupGraphNode* GetSCCNode( GraphAccessInterface::Node *n)
      { return nodeMap.Map(n); }
   GroupGraphCreate* GetGroupGraphCreate() const { return graph; }
  protected:
    void SetCurSCC( GroupGraphNode *scc ) 
        { sccNode = scc; graph->AddGroupNode(sccNode); }
  private:
   GroupGraphNode *sccNode;
   GroupGraphCreate *graph;
   PtrMapWrap <GraphAccessInterface::Node, GroupGraphNode> nodeMap;
};

class DAGBaseGraphImpl;
class ROSE_UTIL_API SCCGraphCreate : public GroupGraphCreate
{
  DAGBaseGraphImpl *impl;
 public:
  SCCGraphCreate( const GraphAccessInterface *orig, SCCGroupGraphOperator *op = 0);
  void TopoSort( bool reverse = false );
};

#endif
