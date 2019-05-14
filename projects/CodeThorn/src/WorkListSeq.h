#ifndef WORKLISTSEQ_H
#define WORKLISTSEQ_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <set>
#include <list>

#include "WorkList.h"

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
#include "WorkListSeq.C"

#endif
