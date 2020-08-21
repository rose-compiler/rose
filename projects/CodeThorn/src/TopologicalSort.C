#include "sage3basic.h"
#include "TopologicalSort.h"

namespace CodeThorn {
  // Author: Markus Schordan, 2020.
  TopologicalSort::TopologicalSort(Labeler& labeler0, Flow& flow0):labeler(labeler0),flow(flow0) {
  }

  std::list<Label> TopologicalSort::topologicallySortedLabelList() {
    semanticRevPostOrderTraversal(flow.getStartLabel());
    return revPostOrderList;
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
      // true edge leads to loop body, sort it first
      semanticRevPostOrderTraversal(flow.outEdgeOfType(lab,EDGE_TRUE).target());
      // false edge exits the loop, sort it after the loop body
      semanticRevPostOrderTraversal(flow.outEdgeOfType(lab,EDGE_FALSE).target());
    } else if(labeler.isFunctionCallLabel(lab)) {
      // call edge leads to function, sort it first
      callLabels.push_back(lab);
      semanticRevPostOrderTraversal(flow.outEdgeOfType(lab,EDGE_CALL).target());
      callLabels.pop_back();
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
      Label callLabel=callLabels.back(); // read only

      //assert(flow.succ(lab).exists(callReturnNode));

      // make sure the traversal returns to the call site. Note, a
      // function is visited at most once. Therefore, all subsequent
      // calls are sorted directly from call-label to callreturn-label
      // even if there is no direct edge
      Label callReturnLabel=labeler.getFunctionCallReturnLabelFromCallLabel(callLabel);
      semanticRevPostOrderTraversal(callReturnLabel);
    } else {
      // for all other nodes use default traversal order
      for(auto slab : flow.succ(lab)) {
        semanticRevPostOrderTraversal(slab);
      }
    }
    revPostOrderList.push_front(lab);
  }

} // end of namespace
