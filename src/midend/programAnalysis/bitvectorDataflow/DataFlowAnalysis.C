#include "DataFlowAnalysis.h"
#include "DGBaseGraphImpl.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

template<class Node, class Data>
DataFlowAnalysis<Node, Data>::DataFlowAnalysis()
  : CFGImplTemplate<Node, CFGEdgeImpl>( new DAGBaseGraphImpl())
{
}

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
      for (NodeIterator pp = this->GetPredecessors(cur); !pp.ReachEnd(); ++pp) {
        Node* pred = *pp;
        Data predout = pred->get_exit_data();
        in = meet_data(in, predout);
      }
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
