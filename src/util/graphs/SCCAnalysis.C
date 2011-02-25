
#include <stdlib.h>
#include <assert.h>
#include <map>

#include <DGBaseGraphImpl.h>
#include <SinglyLinkedList.h>
#include <SCCAnalysis.h>

class LowLinkState {
private:
  struct SCCNodeStatus {
    unsigned int lowlink;
    unsigned int dfnumber;
    bool inStack, mark;

    SCCNodeStatus(unsigned int l=0, unsigned int d=0, bool i=false)
      : lowlink(l), dfnumber(d), inStack(i), mark(false) { }
  };
  std::map <GraphAccessInterface::Node*, SCCNodeStatus,
           std::less <GraphAccessInterface::Node*> > status;
  SinglyLinkedListWrap<GraphAccessInterface::Node*> nodeStack;
  unsigned int count;
public:
  LowLinkState()  : count(0) {}
  ~LowLinkState() { }

  unsigned int Count() { return count; }
  void Mark(GraphAccessInterface::Node *node)
      { ++count;
        status[node].mark = true; }
  bool IsMarked(GraphAccessInterface::Node *node)  { return status[node].mark; }
  unsigned int& LOWLINK(GraphAccessInterface::Node *node)
    { return status[node].lowlink; }
  unsigned int& DFNUMBER(GraphAccessInterface::Node *node)
    { return status[node].dfnumber; }

  void Push(GraphAccessInterface::Node *node)
    { status[node].inStack = true;
      nodeStack.PushFirst(node); }
  GraphAccessInterface::Node *Top()
    { return nodeStack.First()->GetEntry(); }
  GraphAccessInterface::Node* Pop()
    { GraphAccessInterface::Node* node = Top();
      nodeStack.PopFirst();
      status[node].inStack = false;
      return node; }
  bool IsOnStack(GraphAccessInterface::Node *node)
    { return status[node].inStack; }
};

void SCCAnalHelp( const GraphAccessInterface *dg, GraphAccessInterface::Node *v, 
                 LowLinkState& state, SCCAnalOperator& op)
{
  state.Mark(v);                           // mark v
  state.DFNUMBER(v) = state.Count();      // set dfnumber
  state.LOWLINK(v) = state.DFNUMBER(v);   // set lowlink number
  state.Push(v);                           // push v into stack

  for (GraphAccessInterface::EdgeIterator outEdges = dg->GetNodeEdgeIterator(v, GraphAccess::EdgeOut);
       !outEdges.ReachEnd(); ++outEdges) {
    GraphAccessInterface::Edge *e = outEdges.Current();
    GraphAccessInterface::Node* w = dg->GetEdgeEndPoint(e, GraphAccess::EdgeIn);
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
    GraphAccessInterface::Node *x;
    do {
      x = state.Pop();
      op.SCCAdd( x );
    } while ( x != v );
  }
}

void SCCAnalysis :: operator() (const GraphAccessInterface *dg, SCCAnalOperator& op)
{
  operator() (dg, dg->GetNodeIterator(), op);
}

void SCCAnalysis :: operator() (const GraphAccessInterface *dg, 
                                GraphAccessInterface::NodeIterator nodes,
                                SCCAnalOperator& op)
{
    LowLinkState state;
    while (!nodes.ReachEnd()) {
      GraphAccessInterface::Node *node = nodes.Current();
      if ( !state.IsMarked(node) ) {
        SCCAnalHelp( dg, node, state, op );
      }
      ++nodes;
    }
}

void SCCAnalysis :: operator() (const GraphAccessInterface *dg, GraphAccessInterface::Node *node,
                                SCCAnalOperator& op)
{
    LowLinkState state;
    SCCAnalHelp( dg, node, state, op );
}

SCCGraphCreate:: 
SCCGraphCreate( const GraphAccessInterface *orig, SCCGroupGraphOperator* op)
  : GroupGraphCreate(impl = new DAGBaseGraphImpl())
{
  SCCGroupGraphOperator opimpl(this);
  if (op == 0)
    op = &opimpl;
  SCCAnalysis()(orig, *op);
  for (GroupGraphCreate::NodeIterator sccNodes = GetNodeIterator();
       !sccNodes.ReachEnd(); ++sccNodes) {
    GroupGraphNode *scc = sccNodes.Current();
    for (GroupGraphNode::const_iterator nodes=scc->begin();
         ! nodes.ReachEnd(); ++nodes) {
       GraphAccessInterface::Node *src = nodes.Current();
       for (GraphAccessInterface::EdgeIterator edges =
                   orig->GetNodeEdgeIterator( src, GraphAccess::EdgeOut);
            ! edges.ReachEnd(); ++edges) {
         GraphAccessInterface::Edge *e = edges.Current();
         GraphAccessInterface::Node *snk = orig->GetEdgeEndPoint(e, GraphAccess::EdgeIn);
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

void SCCGraphCreate:: TopoSort( bool reverse)
{
  impl->TopoSort(reverse);

//  if (reverse)
//     cerr << "reverse ";
//  cerr << "Topo sort Graph : " << GraphToString(impl) << endl;
}

