

#include "DataFlowAnalysis.h"
#include "DGBaseGraphImpl.h"

template<class Node, class Data>
DataFlowAnalysis<Node, Data>::DataFlowAnalysis()
  : CFGImplTemplate<Node, CFGEdgeImpl>( new DAGBaseGraphImpl())
{
}
// Forward propagation only? Predecessors to successors
template<class Node, class Data>
void DataFlowAnalysis<Node, Data>::
operator()( AstInterface& fa, const AstNodePtr& head)
{
  ROSE_Analysis::BuildCFG( fa, head, *this);
  DAGBaseGraphImpl* base = static_cast<DAGBaseGraphImpl* >(VirtualGraphCreateTemplate<Node, CFGEdgeImpl>::GetBaseGraph());
  base->TopoSort();
  FinalizeCFG( fa);

  bool change = true;
  while (change) {
    change = false;
    for (NodeIterator np = GetNodeIterator(); !np.ReachEnd(); ++np) {
      Node* cur = *np;
      Data inOrig = cur->get_entry_data();
      Data in = inOrig;
      //In[s] = Union of Out[p], where p is a predecessor of s.
      for (NodeIterator pp = GetPredecessors(cur); !pp.ReachEnd(); ++pp) {
	Node* pred = *pp;
	Data predout = pred->get_exit_data();
	in = meet_data(in, predout);
      }
      //Out[s]=Gen[s] Union (In[s]-Kill[s]), s is current node
      if (in != inOrig) {
	cur->set_entry_data(in);
	Data outOrig = cur->get_exit_data();
	cur->apply_transfer_function();
	if (outOrig != cur->get_exit_data())
	  change = true;
      }
    }  
  }
}
