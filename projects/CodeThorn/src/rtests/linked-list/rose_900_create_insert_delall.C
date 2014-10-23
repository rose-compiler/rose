#ifdef PRINT
#include <cstdio>
#endif

class List 
{
  public: List(int _d);
  

  inline void print()
{
    #ifdef PRINT
    #endif
  }
  class List *n;
  int d;
}
;

List::List(int _d) : n(0), d(_d)
{
}
// --------------------------------------

class List *create_iter(int n)
{
  class List *head = 0;
  class List *newElement;
  while(n >= 0){
    newElement = (new List (n));
    (*rts.avpushptr((void**)&(newElement -> n))) = head;
    head = newElement;
    n--;
  }
  return head;
}
// inserts an element into an ascending
// ordered list
// SagivRepsWilhelm98: insert
// changed second parameter:
//   was: pointer to existing elem
//   now: key used to create new elem
// changed return type:
//   was: void
//   now: List* to head of list

class List *insert_iter(class List *head,int k)
{
  class List *cur;
  class List *tail;
  cur = head;
  while(cur -> List::n != 0 && cur -> List::n -> List::d < k){
    cur = cur -> List::n;
  }
  tail = cur -> List::n;
  class List *elem = new List (k);
  (*rts.avpushptr((void**)&(elem -> n))) = tail;
  (*rts.avpushptr((void**)&(cur -> n))) = elem;
  return head;
}
// deletes all elements of a list
// PavluSchordanKrall10: new testcase

void delall_iter(class List *head)
{
  class List *t;
  while(head != 0){
    t = head -> List::n;
    head->~List();rts.registerForCommit((void*)head);
    head = t;
  }
}

class State 
{
  

  public: inline State() : iter(0), list(0), maxlen(100)
{
  }
  int iter;
  class List *list;
  int maxlen;
}
;

void event(class State *s)
{
  if (s -> State::iter == 0 || s -> State::list == 0) {
    (*rts.avpushptr((void**)&(s -> list))) = create_iter(6);
  }
  else {
    if (s -> State::iter < s -> State::maxlen) {
      insert_iter(s -> State::list, s -> State::iter);
    }
    else {
      delall_iter(s -> State::list);
      (*rts.avpushptr((void**)&(s -> list))) = 0;
      (*rts.avpush(&(s -> iter))) = 0;
    }
  }
#ifdef PRINT
#endif
  (*rts.avpush(&(s -> iter)))++;
}
#if 0
#endif
