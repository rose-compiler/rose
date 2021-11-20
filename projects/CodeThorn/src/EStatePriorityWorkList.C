#include "sage3basic.h"
#include "EState.h"
#include "EStatePriorityWorkList.h"
#include "TopologicalSort.h"
#include <iostream>

namespace CodeThorn {
bool operator<(const PriorityElement<EStatePtr>& e1, const PriorityElement<EStatePtr>& e2) { return e1.priority<e2.priority; }
bool operator==(const PriorityElement<EStatePtr>& e1, const PriorityElement<EStatePtr>& e2) { return e1.priority==e2.priority; }
bool operator!=(const PriorityElement<EStatePtr>& c1, const PriorityElement<EStatePtr>& c2) { return !(c1==c2); }
}
  
CodeThorn::EStatePriorityWorkList::EStatePriorityWorkList(TopologicalSort::LabelToPriorityMap map) {
  _labelToPriorityMap=map;
}

std::size_t CodeThorn::EStatePriorityWorkList::size() {
  return _list.size();
}

bool CodeThorn::EStatePriorityWorkList::empty() {
  return _list.empty();
}

void CodeThorn::EStatePriorityWorkList::clear() {
  // fastest method to clear a queue, because it has not clear
  // function this also guarantees to deallocate the memory in
  // constrast to doing a series of pop().
  EStatePriorityWorkList::EStatePriorityQueueType empty;
  std::swap( _list, empty );
}

void CodeThorn::EStatePriorityWorkList::push_front(EStatePtr el) {
  // there is only one push method
  //static int priority=0; // this gives a normal work list (push_back does not work)
  //priority++;
  ROSE_ASSERT(_labelToPriorityMap.size()>0);
  int priority=_labelToPriorityMap[el->label()];
  if(false && priority==0) {
    std::cerr<<"Error: push_front: priority=0 for estate lab:"<<el->label().toString()<<std::endl;
    exit(1);
  }
  _list.push(PriorityElement<EStatePtr>(priority,el));
}

void CodeThorn::EStatePriorityWorkList::push_back(EStatePtr el) {
  // there is only one push method
  push_front(el);
}

const CodeThorn::EState* CodeThorn::EStatePriorityWorkList::front() {
    auto el=_list.top();
    int priority=el.priority;
    //std::cout<<"DEBUG: EPWL: front(): pri:"<<el.priority<<" data:"<<el.data<<std::endl;
    if(false && priority==0) {
      std::cerr<<"Error: push_front: priority=0 for estate lab:"<<el.data->label().toString()<<std::endl;
      
      exit(1);
    }
    return el.data;
}

void CodeThorn::EStatePriorityWorkList::pop_front() {
  // there is only one pop method
  _list.pop();
}
// this return type is only used for compatibility with EStateWorkList. Begin/end do not exist for the pri worklist
CodeThorn::EStateWorkList::iterator CodeThorn::EStatePriorityWorkList::begin() {
  std::cerr<<"Internal Error: EStatePriorityWorkList::begin() not available."<<std::endl;
  exit(1);
}
CodeThorn::EStateWorkList::iterator CodeThorn::EStatePriorityWorkList::end() {
  std::cerr<<"Internal Error: EStatePriorityWorkList::end() not available."<<std::endl;
  exit(1);
}
