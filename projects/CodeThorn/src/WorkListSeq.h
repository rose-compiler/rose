#ifndef WORKLISTSEQ_H
#define WORKLISTSEQ_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <set>
#include <list>

#include "CodeThornException.h"

namespace CodeThorn {

  template <typename Element>
    class WorkListSeq  {
  public:    
    bool isEmpty();
    bool exists(Element elem);
    void add(Element elem);
    void add(std::set<Element>& elemSet);
    Element take();
    Element examine();
    size_t size() { return workList.size(); }
  private:
    std::list<Element> workList;
  };
  
} // end of namespace CodeThorn

// template implementation code
template<typename Element>
bool CodeThorn::WorkListSeq<Element>::isEmpty() { 
  bool res;
  res=(workList.size()==0);
  return res;
}

template<typename Element>
bool CodeThorn::WorkListSeq<Element>::exists(Element elem) {
  typename std::list<Element>::iterator findIter;
  findIter=std::find(workList.begin(), workList.end(), elem);
  return findIter==workList.end();
}

template<typename Element>
void CodeThorn::WorkListSeq<Element>::add(Element elem) { 
  workList.push_back(elem); 
}

template<typename Element>
void CodeThorn::WorkListSeq<Element>::add(std::set<Element>& elemSet) { 
  for(typename std::set<Element>::iterator i=elemSet.begin();i!=elemSet.end();++i) {
    workList.push_back(*i);
  }
}

template<typename Element>
Element CodeThorn::WorkListSeq<Element>::take() {
  if(workList.size()==0) {
    throw CodeThorn::Exception("Error: attempted to take element from empty work list.");
  }  else {
    Element co;
    co=*workList.begin();
    workList.pop_front();
    return co;
  }
}

template<typename Element>
Element CodeThorn::WorkListSeq<Element>::examine() {
  if(workList.size()==0)
    throw CodeThorn::Exception("Error: attempted to examine next element in empty work list.");
  Element elem;
  if(workList.size()>0)
    elem=*workList.begin();
  return elem;
}

#endif
