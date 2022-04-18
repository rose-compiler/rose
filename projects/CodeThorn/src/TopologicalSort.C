#include "sage3basic.h"
#include "TopologicalSort.h"
#include "Labeler.h"
#include "Flow.h"

using namespace std;

namespace CodeThorn {
  TopologicalSort::TopologicalSort(Labeler& labeler0, Flow& flow0):labeler(labeler0),flow(flow0) {
  }
  TopologicalSort::TopologicalSort(Labeler& labeler0, Flow& flow0, Flow* callGraph0):labeler(labeler0),flow(flow0), callGraph(callGraph0) {
  }

  void TopologicalSort::setReverseFunctionOrdering(bool flag) {
    _reverseFunctionOrdering=flag;
  }

  bool TopologicalSort::getReverseFunctionOrdering() {
    return _reverseFunctionOrdering;
  }

  LabelSet TopologicalSort::unreachableLabels() {
    return _unreachable;
  }

  void TopologicalSort::createTopologicallySortedLabelList() {
    if(revPostOrderList.size()==0) {
      Label startLab=flow.getStartLabel();
      ROSE_ASSERT(startLab.isValid());
      // use call graph to compute post order (reverse topsort) and compute top sort for each function in this order
      if(callGraph) {
        std::list<Label> postOrderList;
        computePostOrder(startLab,*callGraph, postOrderList);
        // reverse post order sorting of functions (each function is also topologically sorted)
        if(getReverseFunctionOrdering()) {
          for (auto revit=postOrderList.rbegin(); revit!=postOrderList.rend();++revit) {
            Label label=*revit;
            semanticRevPostOrderTraversal(label);
          }
        } else {
          for (auto it=postOrderList.begin(); it!=postOrderList.end();++it) {
            Label label=*it;
            semanticRevPostOrderTraversal(label);
          }
        }
      } else {
        // likely inefficient, without call graph, top-sort functions starting with main and then in some "random" order
        if(startLab.isValid()) {
          semanticRevPostOrderTraversal(startLab);
        }
      }

      // store revlist for all labels reachable from start and reset result list
      std::list<Label> revPostOrderListStart=revPostOrderList;
      revPostOrderList.clear();
      
      // compute top sort for all functions that have not been considered yet
      for(auto iter=flow.nodes_begin();iter!=flow.nodes_end();++iter) {
        Label lab=*iter;
        if(labeler.isFunctionEntryLabel(lab)) {
          semanticRevPostOrderTraversal(lab);
        }
      }
      // do one more traversal to find unreachable labels. Those labels are not included in the topsort, because they can never be reached in the provided Flow graph.
      for(auto iter=flow.nodes_begin();iter!=flow.nodes_end();++iter) {
        Label lab=*iter;
        if(!visited[lab]) {
          //cout<<"TopSort: found unreachable label:"<<lab.toString()<<": "<<labeler.sourceLocationToString(lab,40,30)<<endl;
          _unreachable.insert(lab);
          semanticRevPostOrderTraversal(lab);
        }
      }

      // insert revPostOrderList with start node (at beginning) in revlist of non-reachable subgraphs (otherwise start is not first if unreachable nodes exist)
      revPostOrderList.insert(revPostOrderList.begin(),revPostOrderListStart.begin(),revPostOrderListStart.end());
    }
  }

  bool TopologicalSort::isRecursive() {
    return _recursiveCG;
  }
  
  void TopologicalSort::computePostOrder(Label lab, Flow& flow, std::list<Label>& list) {
    std::set<Label> visited;
    // check special case, if lab is in flow (e.g. start label
    // represents main, but no function is called from main. Then
    // there is no call edge, and the start node not in the call
    // graph.
    if(flow.contains(lab)) {
      computePostOrder(lab,flow,list,visited);
    } else {
      //cout<<"DEBUG: special case start label not in flow."<<endl;
    }
  }
  
  void TopologicalSort::computePostOrder(Label lab, Flow& flow, std::list<Label>& list, std::set<Label>& visited) {
    ROSE_ASSERT(lab.isValid());
    if(visited.find(lab)==visited.end()) {
      visited.insert(lab);
      auto Succ=flow.succ(lab);
      for (auto slab : Succ) {
        computePostOrder(slab,flow, list, visited);
      }
    } else {
      _recursiveCG=true;
    }
    list.push_back(lab);
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
    return _map;
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
      // do no inter-proc top sort, only sort each function, therefore go to callreturn lab immediately
      Label callReturnLabel=labeler.getFunctionCallReturnLabelFromCallLabel(lab);
      if(callReturnLabel.isValid()) {
        semanticRevPostOrderTraversal(callReturnLabel);
      }
    } else if(labeler.isFunctionExitLabel(lab)) {
      // sorting has to end at exit label because it is done for each function separately
      // label is entered in result list at the end of function (post-order)
    } else {
      // for all other nodes use default traversal order
      for(auto slab : flow.succ(lab)) {
        semanticRevPostOrderTraversal(slab);
      }
    }
    revPostOrderList.push_front(lab);
  }
} // end of namespace
