#include "sage3basic.h"
#include "TopologicalSort.h"

using namespace std;

namespace CodeThorn {
  // Author: Markus Schordan, 2020.
  TopologicalSort::TopologicalSort(Labeler& labeler0, Flow& flow0):labeler(labeler0),flow(flow0) {
  }

  void TopologicalSort::createTopologicallySortedLabelList() {
    if(revPostOrderList.size()==0) {
      LabelSet slabs=flow.getStartLabelSet();
      for(auto slab : slabs) {
        semanticRevPostOrderTraversal(slab);
      }
    }
  }
  
  std::list<Label> TopologicalSort::topologicallySortedLabelList() {
    createTopologicallySortedLabelList();
    return revPostOrderList;
  }

  TopologicalSort::LabelToPriorityMap TopologicalSort::labelToPriorityMap() {
    TopologicalSort::LabelToPriorityMap map;
    createTopologicallySortedLabelList();
    int i=1;
    for(auto lab : revPostOrderList) {
      map[lab]=i;
      i++;
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
#if 1
    if(labeler.isLoopConditionLabel(lab)) {
      // true edge leads to loop body, sort it first
      semanticRevPostOrderTraversal(flow.outEdgeOfType(lab,EDGE_TRUE).target());
      // false edge exits the loop, sort it after the loop body
      semanticRevPostOrderTraversal(flow.outEdgeOfType(lab,EDGE_FALSE).target());
    } else if(labeler.isFunctionCallLabel(lab)) {
      // call edge leads to function, sort it first
      Flow callFlow=flow.outEdgesOfType(lab,EDGE_CALL);
      for(auto callEdge:callFlow) {
        callLabels.push_back(lab);
        semanticRevPostOrderTraversal(callEdge.target());
        callLabels.pop_back();
      }
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
        Label callReturnLabel=labeler.getFunctionCallReturnLabelFromCallLabel(callLabel);
        LabelSet succ=flow.succ(lab);
        if(callReturnLabel.isValid()) {
          semanticRevPostOrderTraversal(callReturnLabel);
          // remove the one call label that is already traversed
          succ.erase(callReturnLabel);
          //cout<<"DEBUG: @exit: "<<callLabel.toString()<<","<<callReturnLabel.toString()<<endl;
        }
        //ROSE_ASSERT(succ.find(callReturnLabel)!=succ.end());
        // traverse all other outgoing edges of exit node
        for(auto slab : succ) {
          semanticRevPostOrderTraversal(slab);
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
#else
  for(auto slab : flow.succ(lab)) {
    semanticRevPostOrderTraversal(slab);
  }
  revPostOrderList.push_front(lab);
#endif  
  }
} // end of namespace
