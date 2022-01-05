#include "sage3basic.h"
#include "GeneralPriorityWorkList.h"
#include "TopologicalSort.h"
#include <iostream>


namespace CodeThorn {
  template<typename Element>
bool operator<(const GeneralPriorityElement<Element>& e1, const GeneralPriorityElement<Element>& e2) { return e1.priority<e2.priority; }
  template<typename Element>
bool operator==(const GeneralPriorityElement<Element>& e1, const GeneralPriorityElement<Element>& e2) { return e1.priority==e2.priority; }
  template<typename Element>
bool operator!=(const GeneralPriorityElement<Element>& c1, const GeneralPriorityElement<Element>& c2) { return !(c1==c2); }
}

template<typename Element>
CodeThorn::GeneralPriorityWorkList<Element>::GeneralPriorityWorkList(TopologicalSort::LabelToPriorityMap map) {
  _labelToPriorityMap=map;
}

template<typename Element>
std::size_t CodeThorn::GeneralPriorityWorkList<Element>::size() {
  return _list.size();
}

template<typename Element>
bool CodeThorn::GeneralPriorityWorkList<Element>::empty() {
  return _list.empty();
}

template<typename Element>
void CodeThorn::GeneralPriorityWorkList<Element>::clear() {
  // fastest method to clear a queue, because it has not clear
  // function this also guarantees to deallocate the memory in
  // constrast to doing a series of pop().
  typename GeneralPriorityWorkList::ElementPriorityQueueType empty;
  std::swap( _list, empty );
}

template<typename Element>
void CodeThorn::GeneralPriorityWorkList<Element>::push(Element el) {
  ROSE_ASSERT(_labelToPriorityMap.size()>0);
  int priority=_labelToPriorityMap[el.label()];
  if(false && priority==0) {
    std::cerr<<"Error: push_front: priority=0 for label:"<<el.label().toString()<<std::endl;
    exit(1);
  }
  _list.push(GeneralPriorityElement<Element>(priority,el));
}

template<typename Element>
Element CodeThorn::GeneralPriorityWorkList<Element>::top() {
    auto priElem=_list.top();
    int priority=priElem.priority;
    //std::cout<<"DEBUG: EPWL: front(): pri:"<<el.priority<<" data:"<<el.data<<std::endl;
    if(false && priority==0) {
      std::cerr<<"Error: push_front: priority=0 for estate lab:"<<priElem.data.label().toString()<<std::endl;
      
      exit(1);
    }
    return priElem.data;
}

template<typename Element>
void CodeThorn::GeneralPriorityWorkList<Element>::pop() {
  // there is only one pop method since the work list is priority list
  _list.pop();
}

template<typename Element>
void CodeThorn::GeneralPriorityWorkList<Element>::push_front(Element el) {
  push(el);
}

template<typename Element>
void CodeThorn::GeneralPriorityWorkList<Element>::push_back(Element el) {
  // there is only one push method since the work list is priority list
  push(el);
}

template<typename Element>
Element CodeThorn::GeneralPriorityWorkList<Element>::front() {
  return top();
}

template<typename Element>
void CodeThorn::GeneralPriorityWorkList<Element>::pop_front() {
  pop();
}

