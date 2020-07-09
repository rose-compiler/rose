#ifndef WORKLISTSEQ_H
#define WORKLISTSEQ_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <set>
#include <deque>
//~ #include <list>

#include "WorkList.h"
#include "CodeThornException.h"

namespace CodeThorn {

  template <typename Element>
  class WorkListSeq  {
      //~ std::list<Element> workList;
      std::deque<Element> workList;
  
  public:    
    typedef Element value_type;
  
    bool isEmpty() const;
    bool exists(Element elem) const;
    void add(Element elem);
    void add(std::set<Element>& elemSet);
    Element take();
    Element examine() const;
    size_t size() const { return workList.size(); }
    
    auto begin() const -> decltype( workList.begin() ) 
    { return workList.begin(); }
    
    auto end() const -> decltype( workList.end() ) 
    { return workList.end(); }
  };
  
} // end of namespace CodeThorn

// template implementation code
template<typename Element>
bool CodeThorn::WorkListSeq<Element>::isEmpty() const { 
  bool res;
  res=(workList.size()==0);
  return res;
}

template<typename Element>
bool CodeThorn::WorkListSeq<Element>::exists(Element elem) const {
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
  if(workList.size()==0) 
    throw CodeThorn::Exception("Error: attempted to take element from empty work list.");

  Element co = workList.front();
  workList.pop_front();
  return co;
}

template<typename Element>
Element CodeThorn::WorkListSeq<Element>::examine() const {
  if(workList.size()==0)
    throw CodeThorn::Exception("Error: attempted to examine next element in empty work list.");
    
  return workList.front();
}

#endif
