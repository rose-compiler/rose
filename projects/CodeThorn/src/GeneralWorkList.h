#ifndef GeneralWorkList_H
#define GeneralWorkList_H

#include <list>
#include <cstddef>

namespace CodeThorn {

template <typename Element>
  class GeneralWorkList {
 public:
 protected:
  typedef typename std::list<Element> ContainerType;
  ContainerType _list;
 public:
  virtual bool empty();
  virtual void push_front(Element el);
  virtual void pop_front();
  virtual Element front();
  virtual void push_back(Element);
  virtual std::size_t size();
  virtual void clear();
  virtual auto begin() const ->decltype(_list.begin());
  virtual auto end() const ->decltype(_list.end());
  virtual ~GeneralWorkList();
 };
#include "GeneralWorkList.C"
}

#endif
