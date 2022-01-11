#include "sage3basic.h"
#include "TopologicalSort.h"
#include "Labeler.h"
#include "Flow.h"

using namespace std;

namespace CodeThorn {
  // Author: Markus Schordan, 2020.
  TopologicalSort::TopologicalSort(Labeler& labeler0, Flow& flow0):labeler(labeler0),flow(flow0) {
  }

  void TopologicalSort::createTopologicallySortedLabelList() {
    if(revPostOrderList.size()==0) {
      // determine nodes with no predecessors (this is sufficient to find all entry nodes including disconnected subgraphs)
      for(auto iter=flow.nodes_begin();iter!=flow.nodes_end();++iter) {
        Label lab=*iter;
        if(labeler.isFunctionEntryLabel(lab) && flow.pred(lab).size()==0) {
          semanticRevPostOrderTraversal(lab);
        }
      }
    }
  }

  uint32_t TopologicalSort::getLabelPosition(Label lab) const {
    auto iter=_map.find(lab);
    if(iter==_map.end())
      return 0;
    else {
      return (*iter).second;
    }
  }
  
  std::list<Label> TopologicalSort::topologicallySortedLabelList() {
    if(revPostOrderList.size()==0)
      createTopologicallySortedLabelList();
    return revPostOrderList;
  }

  void TopologicalSort::computeLabelToPriorityMap() {
    createTopologicallySortedLabelList();
    int i=1;
    for(auto lab : revPostOrderList) {
      _map[lab]=i;
      i++;
    }
  }
  TopologicalSort::LabelToPriorityMap TopologicalSort::labelToPriorityMap() {
    LabelToPriorityMap map;
    if(_map.size()==0)
      computeLabelToPriorityMap();
    for(auto e : _map) {
      map[e.first]=map[e.second];
    }
    return map;
  }
    
  // computes reverse post-order of labels in revPostOrderList
  void TopologicalSort::semanticRevPostOrderTraversal(Label lab) {

    // this is used to allow for invalid edges (whoes target is an invalid labell)
    if(!lab.isValid())
      return;

    if(visited[lab]) {
      return;
    } else {
      visited[lab]=true;
    }

    if(labeler.isLoopConditionLabel(lab)) {
      // false edge exits the loop, sort it after the loop body
      semanticRevPostOrderTraversal(flow.outEdgeOfType(lab,EDGE_FALSE).target());
      // true edge leads to loop body, sort it first
      semanticRevPostOrderTraversal(flow.outEdgeOfType(lab,EDGE_TRUE).target());
    } else if(labeler.isFunctionCallLabel(lab)) {
      // call edge leads to function, sort it first
      Flow callFlow=flow.outEdgesOfType(lab,EDGE_CALL);
      if(callFlow.size()==1) {
	for(auto callEdge:callFlow) {
	  callLabels.push_back(lab);
	  semanticRevPostOrderTraversal(callEdge.target());
	  if(callLabels.back()==lab)
	    callLabels.pop_back();
	  Label callReturnLabel=labeler.getFunctionCallReturnLabelFromCallLabel(lab);
	  semanticRevPostOrderTraversal(callReturnLabel);
	}
      }
      if(callFlow.size()>1) {
	// handle callreturn node for multi-target nodes (which are skipped)
	Label callReturnLabel=labeler.getFunctionCallReturnLabelFromCallLabel(lab);
	if(callReturnLabel.isValid()) {
	  semanticRevPostOrderTraversal(callReturnLabel);
	}
      }
      // TODO: what about callReturn labels of multi-calls?
      Label callReturnLabel=flow.outEdgeOfType(lab,EDGE_EXTERNAL).target();
      if(callReturnLabel.isValid()) {
        semanticRevPostOrderTraversal(callReturnLabel);
      }
    } else if(labeler.isFunctionExitLabel(lab)) {

      // exit node needs to match the corresponding call node since
      // every function is visited at most once, a stack is sufficient
      // to track this information read only to obtain label. The
      // label is removed from the list when the traversal returns
      // from the call (see above case for FunctionCallLabel)
      if(callLabels.size()>0) {
        Label callLabel=callLabels.back(); // read only
        // make sure the traversal returns to the call site. Note, a
        // function is visited at most once.
	callLabels.pop_back();
        Label callReturnLabel=labeler.getFunctionCallReturnLabelFromCallLabel(callLabel);
        LabelSet succ=flow.succ(lab);
        if(callReturnLabel.isValid()) {
	  // only continue for callreturn label, all others will be visited from other calls
          semanticRevPostOrderTraversal(callReturnLabel);
        }
      } else {
        // in case the call context has length zero
        for(auto slab : flow.succ(lab)) {
          semanticRevPostOrderTraversal(slab);
        }
      }
    } else {
      // for all other nodes use default traversal order
      for(auto slab : flow.succ(lab)) {
        semanticRevPostOrderTraversal(slab);
      }
    }
    revPostOrderList.push_front(lab);
  }
} // end of namespace
