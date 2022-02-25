#ifndef TOPOLOGICAL_SORT
#define TOPOLOGICAL_SORT

#include <list>
#include <map>
#include "Label.h"

namespace CodeThorn {

  class Labeler;
  class Flow;
  class Label;
  
  // Author: Markus Schordan, 2020.
  class TopologicalSort {
  public:
    typedef std::map<Label,uint32_t> LabelToPriorityMap;
    TopologicalSort(Labeler& labeler, Flow& flow);
    TopologicalSort(Labeler& labeler, Flow& flow, Flow* callGraph);
    void computeLabelToPriorityMap();
    uint32_t getLabelPosition(Label lab) const;

    std::list<Label> topologicallySortedLabelList();
    LabelToPriorityMap labelToPriorityMap();

  private:

    // computes reverse post-order of labels in revPostOrderList
    void semanticRevPostOrderTraversal(Label lab);
    void createTopologicallySortedLabelList();

    Labeler& labeler;
    Flow& flow;
    Flow* callGraph=nullptr;
    std::map<Label,bool> visited;
    std::list<Label> callLabels;
    std::list<Label> revPostOrderList;
    LabelToPriorityMap _map;
  };

} // end of namespace CodeThorn

#endif
