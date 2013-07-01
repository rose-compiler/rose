#ifndef WORKLIST_H
#define WORKLIST_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <set>

namespace CodeThorn {
template <typename Element>
class WorkList {
 public:	
  bool isEmpty();
  bool exists(Element elem);
  void add(Element elem);
  void add(std::set<Element>& elemSet);
  Element take();
  Element examine();
 private:
  list<Element> workList;
};

} // end of namespace CodeThorn

#endif
