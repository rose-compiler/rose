#ifndef EStatePriorityWorkList_H
#define EStatePriorityWorkList_H

#include <queue>
#include <string>
#include <sstream>
#include <vector>
#include <cstddef>
#include "EStateWorkList.h"
#include "TopologicalSort.h"

// Author: Markus Schordan, 2020.

namespace CodeThorn {

  class EState;

  template<typename T>
struct PriorityElement {
  PriorityElement(int priority, T data):priority(priority),data(data){}
  std::string toString() const {
    std::stringstream ss;
    ss<<"(pri:"<<priority<<",data:"<<data<<")";
    return ss.str();
  }
  int priority;
  T data;
  ~PriorityElement() {
    if(std::is_pointer<T>::value) {
      //cout<<"is pointer"<<endl;
    }
  }
};

  bool operator<(const PriorityElement<const EState*>& e1, const PriorityElement<const EState*>& e2);
  bool operator==(const PriorityElement<const EState*>& e1, const PriorityElement<const EState*>& e2);
  bool operator!=(const PriorityElement<const EState*>& c1, const PriorityElement<const EState*>& c2);

  // functor object to provide greater operator as predicate
  struct PriorityElementGreaterOp {
    bool operator()(const PriorityElement<const EState*>& e1, const PriorityElement<const EState*>& e2) {
      return !(e1==e2||e1<e2);
    }
  };
  
  class EStatePriorityWorkList : public EStateWorkList {
  public:
    EStatePriorityWorkList(CodeThorn::TopologicalSort::LabelToPriorityMap map);
    bool empty();
    void push_front(const EState* el);
    void pop_front();
    const EState* front();
    void push_back(const EState*);
    std::size_t size();
    void clear();
    EStateWorkList::iterator begin();
    EStateWorkList::iterator end();
    typedef PriorityElement<const EState*> EStatePriElem;
    // min priority queue (smaller elements are orderered first)
    typedef std::priority_queue <EStatePriElem,std::vector<EStatePriElem>,PriorityElementGreaterOp> EStateMinPriorityQueueType;
    //typedef std::priority_queue <EStatePriElem> EStateMaxPriorityQueueType;
    typedef EStateMinPriorityQueueType EStatePriorityQueueType;
 protected:
    TopologicalSort::LabelToPriorityMap _labelToPriorityMap;
    EStatePriorityQueueType _list;
  };
}

#endif
