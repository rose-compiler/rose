#include <general.h>
#include <stdlib.h>
#include <assert.h>
#include <map>

// pmp 08JUN05
//   cmp comment in ./IDGraphCreate.h
#include <iostream>

#include <SinglyLinkedList.h>
#include <GraphInterface.h>
#include <SCCAnalysis.h>
#include <DGBaseGraphImpl.h>

class LowLinkState {
private:
  struct SCCNodeStatus {
    unsigned int lowlink;
    unsigned int dfnumber;
 // Boolean inStack, mark;
    int inStack, mark;

 // SCCNodeStatus(unsigned int l=0, unsigned int d=0, Boolean i=false)
    SCCNodeStatus(unsigned int l=0, unsigned int d=0, int i=false)
      : lowlink(l), dfnumber(d), inStack(i), mark(false) { }
  };
  std::map <GraphNode*, SCCNodeStatus, std::less <GraphNode*> > status;
  SinglyLinkedListWrap<GraphNode*> nodeStack;
  unsigned int count;
public:
  LowLinkState()  : count(0) {}
  ~LowLinkState() { }
  
  unsigned int Count() { return count; }
  void Mark(GraphNode *node)  
      { ++count; 
        status[node].mark = true; }
//Boolean IsMarked(GraphNode *node)  { return status[node].mark; }
  int IsMarked(GraphNode *node)  { return status[node].mark; }
  unsigned int& LOWLINK(GraphNode *node) 
    { return status[node].lowlink; }
  unsigned int& DFNUMBER(GraphNode *node) 
    { return status[node].dfnumber; }

  void Push(GraphNode *node) 
    { status[node].inStack = true;
      nodeStack.PushFirst(node); }
  GraphNode *Top() 
    { return nodeStack.First()->GetEntry(); }
  GraphNode* Pop() 
    { GraphNode* node = Top();
      nodeStack.PopFirst(); 
      status[node].inStack = false;
      return node; }
//Boolean IsOnStack(GraphNode *node) 
  int IsOnStack(GraphNode *node) 
    { return status[node].inStack; }
};

#ifndef TEMPLATE_ONLY
void SCCAnalHelp( const GraphAccess *dg, GraphNode *v, LowLinkState& state,
         SCCAnalOperator& op)
{
  state.Mark(v);                           // mark v
  state.DFNUMBER(v) = state.Count();      // set dfnumber
  state.LOWLINK(v) = state.DFNUMBER(v);   // set lowlink number
  state.Push(v);                           // push v into stack

  for (GraphAccess::EdgeIterator outEdges = dg->GetNodeEdgeIterator(v, GraphAccess::EdgeOut);
       !outEdges.ReachEnd(); ++outEdges) {
    GraphEdge *e = outEdges.Current();
    GraphNode* w = dg->GetEdgeEndPoint(e, GraphAccess::EdgeIn);
    if (!state.IsMarked(w)) { 
      SCCAnalHelp(dg, w, state, op);
      if (state.LOWLINK(v) > state.LOWLINK(w))
         state.LOWLINK(v) = state.LOWLINK(w);
    } 
    else {
      if ( state.DFNUMBER(w) < state.LOWLINK(v) &&  state.IsOnStack(w) ) {
     	state.LOWLINK(v) = state.DFNUMBER(w);
      }
    }
  }

  // if lowlink[v] == DFNumber[v], the current Node in stack above
  // v form a SCC. 
  if ( state.LOWLINK(v) == state.DFNUMBER(v) ) {
    op.CreateSCC();
    GraphNode *x;
    do {
      x = state.Pop();
      op.SCCAdd( x );
    } while ( x != v );
  }
}
#endif

#ifndef TEMPLATE_ONLY
void SCCAnalysis :: operator() (const GraphAccess *dg, SCCAnalOperator& op)
{
  operator() (dg, dg->GetNodeIterator(), op);
}
#endif

#ifndef TEMPLATE_ONLY
void SCCAnalysis :: operator() (const GraphAccess *dg, GraphAccess::NodeIterator nodes,
                                SCCAnalOperator& op)
{
    LowLinkState state;
    while (!nodes.ReachEnd()) {
      GraphNode *node = nodes.Current();
      if ( !state.IsMarked(node) ) {
        SCCAnalHelp( dg, node, state, op );
      }
      ++nodes;
    }
}
#endif

#ifndef TEMPLATE_ONLY
void SCCAnalysis :: operator() (const GraphAccess *dg, GraphNode *node,
                                SCCAnalOperator& op)
{
    LowLinkState state;
    SCCAnalHelp( dg, node, state, op );
}
#endif

#ifndef TEMPLATE_ONLY
SCCGraphCreate:: SCCGraphCreate( const GraphAccess *orig, SCCGroupGraphOperator* op)
  : GroupGraphCreate(impl = new DAGBaseGraphImpl())
{
  SCCGroupGraphOperator opimpl(this);
  if (op == 0)
    op = &opimpl;
  SCCAnalysis()(orig, *op);
  for (GroupGraphCreate::NodeIterator sccNodes = GetNodeIterator();
       !sccNodes.ReachEnd(); ++sccNodes) {
    GroupGraphNode *scc = sccNodes.Current();
    for (GroupGraphNode::Iterator nodes=scc->GetIterator();
         ! nodes.ReachEnd(); ++nodes) {
       GraphNode *src = nodes.Current();
       for (GraphAccess::EdgeIterator edges =
                   orig->GetNodeEdgeIterator( src, EdgeOut);
            ! edges.ReachEnd(); ++edges) {
         GraphEdge *e = edges.Current();
         GraphNode *snk = orig->GetEdgeEndPoint(e, EdgeIn);
         GroupGraphNode *scc1 = op->GetSCCNode(snk);
         if (scc != scc1) {
           GroupGraphEdge * sccEdge = new GroupGraphEdge(this);
           AddGroupEdge( scc, scc1, sccEdge);
           sccEdge->AddEdge( e );
         }
       }
    }
  }
}
#endif

#ifndef TEMPLATE_ONLY
// void SCCGraphCreate:: TopoSort( Boolean reverse)
void SCCGraphCreate:: TopoSort( int reverse)
{
  impl->TopoSort(reverse);

//  if (reverse)
//     cerr << "reverse ";
//  cerr << "Topo sort Graph : " << GraphToString(impl) << endl;
}
#endif

#define TEMPLATE_ONLY
#include <IDGraphCreate.C>
template class IDGraphCreateTemplate<GroupGraphNode, GroupGraphEdge>;
