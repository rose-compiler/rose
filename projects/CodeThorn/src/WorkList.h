#ifndef WORKLIST_H
#define WORKLIST_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

namespace CodeThorn {
template <typename Element>
class WorkList {
 public:	
  bool isEmpty();
  bool exists(Element estate);
  void add(Element estate);
  Element take();
  Element examine();
 private:
  list<Element> workList;
};

} // end of namespace CodeThorn

#endif
