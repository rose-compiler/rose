#include "sage3basic.h"
#include "GeneralPriorityWorkList.h"
#include "TopologicalSort.h"
#include <iostream>


namespace CodeThorn {
  template<typename Element>
  bool operator<(const GeneralPriorityElement<Element>& e1, const GeneralPriorityElement<Element>& e2) {
    if(e1.priority!=e2.priority)
      return e1.priority<e2.priority;
    else
      return e1.data.callString()<e2.data.callString(); // if priority is the same the cs defines the order
  }
  template<typename Element>
  bool operator==(const GeneralPriorityElement<Element>& e1, const GeneralPriorityElement<Element>& e2) {
    bool result=e1.priority==e2.priority && (e1.data.callString()==e2.data.callString())<<std::endl;
    return e1.priority==e2.priority && (e1.data.callString()==e2.data.callString());
  }
  template<typename Element>
  bool operator!=(const GeneralPriorityElement<Element>& c1, const GeneralPriorityElement<Element>& c2) {
    return !(c1==c2);
  }
}

template<typename Element>
CodeThorn::GeneralPriorityWorkList<Element>::GeneralPriorityWorkList(TopologicalSort::LabelToPriorityMap map) {
  _labelToPriorityMap=map;
  //std::cout<<"DEBUG: generating GenPriWorkList (label to priority map):"<<std::endl;
  //for(auto e:map) {
  //  std::cout<<"DEBUG: l2p map entry: "<<e.first.toString()<<":"<<e.second<<std::endl;
  //}
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
  if(priority==0) {
    std::cerr<<"Warning: push_front: priority=0 for label:"<<el.label().toString()<<std::endl;
    //exit(1);
  }
  _list.insert(GeneralPriorityElement<Element>(priority,el));
}

template<typename Element>
Element CodeThorn::GeneralPriorityWorkList<Element>::top() {
  auto priElem=*_list.begin();
  int priority=priElem.priority;
  //std::cout<<"DEBUG: EPWL: front(): pri:"<<el.priority<<" data:"<<el.data<<std::endl;
  if(false && priority==0) {
    std::cerr<<"Error: top: priority=0 for estate lab:"<<priElem.data.label().toString()<<std::endl;
    exit(1);
  }
  return priElem.data;
}

template<typename Element>
void CodeThorn::GeneralPriorityWorkList<Element>::pop() {
  // there is only one pop method since the work list is priority list
  _list.erase(_list.begin());
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

template<typename Element>
void CodeThorn::GeneralPriorityWorkList<Element>::print() {
  std::cout<<"WL: "<<size()<<": [";
  for(auto p : _list) {
    std::cout<<p.toString()<<", ";
  }
  std::cout<<std::endl;
}

