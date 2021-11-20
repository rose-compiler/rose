#ifndef EStateWorkList_H
#define EStateWorkList_H

#include <list>
#include <cstddef>

#include "EState.h"

namespace CodeThorn {

  class EState;

  class EStateWorkList {
  public:
    typedef std::list<EStatePtr>::iterator iterator;
    virtual bool empty();
    virtual void push_front(EStatePtr el);
    virtual void pop_front();
    virtual EStatePtr front();
    virtual void push_back(EStatePtr);
    virtual std::size_t size();
    virtual void clear();
    virtual iterator begin();
    virtual iterator end();
    virtual ~EStateWorkList();
  protected:
    std::list<EStatePtr> _list;
  };
}

#endif
