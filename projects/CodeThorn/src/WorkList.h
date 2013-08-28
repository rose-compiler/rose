#ifndef WORKLIST_H
#define WORKLIST_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <set>
#include <list>

namespace CodeThorn {
template <typename Element>
class WorkList {
 public:    
  virtual bool isEmpty()=0;
  virtual bool exists(Element elem)=0;
  virtual void add(Element elem)=0;
  virtual void add(std::set<Element>& elemSet)=0;
  virtual Element take()=0;
  virtual Element examine()=0;
 private:
  // container provided by implementing class
};

} // end of namespace CodeThorn

#endif
