#ifndef TOPOLOGICAL_SORT
#define TOPOLOGICAL_SORT

#include "Labeler.h"
#include "Flow.h"
#include <list>
#include <map>

namespace CodeThorn {

// use as:
// TopologicalSort topSort(labeler,flow);
// std::list<Label> list=topSort.topologicallySortedLabelList();

// Author: Markus Schordan, 2020.
class TopologicalSort {
 public:
  typedef std::map<Label,int> LabelToPriorityMap;
  TopologicalSort(Labeler& labeler0, Flow& flow0);
  std::list<Label> topologicallySortedLabelList();
  LabelToPriorityMap labelToPriorityMap();
 private:
  // computes reverse post-order of labels in revPostOrderList
  void semanticRevPostOrderTraversal(Label lab);

 private:
  Labeler& labeler;
  Flow& flow;
  std::map<Label,bool> visited;
  std::list<Label> callLabels;
  std::list<Label> revPostOrderList;
};

} // end of namespace CodeThorn

#endif
