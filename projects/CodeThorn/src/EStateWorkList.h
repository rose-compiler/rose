#ifndef EStateWorkList_H
#define EStateWorkList_H

#include <list>


namespace CodeThorn {

  class EState;

  class EStateWorkList {
  public:
    typedef std::list<const EState*>::iterator iterator;
    bool empty();
    void push_front(const EState* el);
    void pop_front();
    const EState* front();
    void push_back(const EState*);
    size_t size();
    void clear();
    iterator begin();
    iterator end();
  private:
    std::list<const EState*> _list;
  };
}

#endif
