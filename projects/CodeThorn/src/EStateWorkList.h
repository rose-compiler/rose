#ifndef EStateWorkList_H
#define EStateWorkList_H

#include <list>

namespace CodeThorn {

  class EState;

  class EStateWorkList {
  public:
    typedef std::list<const EState*>::iterator iterator;
    virtual bool empty();
    virtual void push_front(const EState* el);
    virtual void pop_front();
    virtual const EState* front();
    virtual void push_back(const EState*);
    virtual size_t size();
    virtual void clear();
    virtual iterator begin();
    virtual iterator end();
    virtual ~EStateWorkList();
  protected:
    std::list<const EState*> _list;
  };
}

#endif
