#ifndef EStateWorkList_H
#define EStateWorkList_H

#include <list>
#include <cstddef>

#include "EState.h"
#include "GeneralWorkList.h"

namespace CodeThorn {

  class EState;

  class EStateWorkList : public GeneralWorkList<EStatePtr> {
  public:
    virtual bool empty();
    virtual void push_front(EStatePtr el);
    virtual void pop_front();
    virtual EStatePtr front();
    virtual void push_back(EStatePtr);
    virtual std::size_t size();
    virtual void clear();
    virtual ~EStateWorkList();
  protected:
  };
}

#endif
