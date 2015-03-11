#ifdef PRINT
#include <cstdio>
#endif

class A 
{
  public: int val;
}
;

class B : public A
{
}
;
template < typename T >
class List {
public :
  List ( T _d );
  void print ( );
  List * n;
  T d;
};
template < typename T >
List < T > :: List ( T _d ) : n ( 0 ), d ( _d ) { }
// --------------------------------------
// creates a list of length n+1
template < typename T >
List < T > * create_iter ( int n ) {
  List < T > * head = 0;
  List < T > * newElement;
  while ( n >= 0 ) {
    A * a = new A ( );
    a -> val = n;
    newElement = new List < T > ( a );
    newElement -> n = head;
    head = newElement;
    n --;
  }
  return head;
}
// inserts an element into an ascending list
template < typename T >
List < T > * insert_iter ( List < T > * head, T k ) {
  List < T > * cur;
  List < T > * tail;
  cur = head;
  while ( cur -> n != 0 && ( cur -> n -> d -> val < k -> val ) ) {
    cur = cur -> n;
  }
  tail = cur -> n;
  List < T > * elem = new List < T > ( k );
  elem -> n = tail;
  cur -> n = elem;
  return head;
}
// deletes all elements of a list
template < typename T >
void delall_iter ( List < T > * head ) {
  List < T > * t;
  while ( head != 0 ) {
    t = head -> n;
    delete head;
    head = t;
  }
}

class State 
{
  

  public: inline State() : iter(0), list(0), maxlen(0)
{
  }
  int iter;
  class List< A * > *list;
  int maxlen;
}
;

void event(class State *s)
{
  if (s -> State::iter == 0 || s -> State::list == 0) {
    (*rts.avpush(&(s -> list))) = ::create_iter< A * > (6);
  }
  else {
    if (s -> State::iter < s -> State::maxlen) {
      class A *a;
      if (s -> State::iter % 2 == 0) {
        (*rts.avpush(&(a))) = (new A ());
      }
      else {
        (*rts.avpush(&(a))) = (new B ());
      }
      (*rts.avpush(&(a -> val))) = s -> State::iter;
      ::insert_iter< A * > (s -> State::list,a);
    }
    else {
      ::delall_iter< A * > (s -> State::list);
      (*rts.avpush(&(s -> list))) = 0;
      (*rts.avpush(&(s -> iter))) = 0;
    }
  }
#ifdef PRINT
#endif
  (*rts.avpush(&(s -> iter)))++;
}

int main(int argc,char **argv)
{
  class State *s = new State ();
  (*rts.avpush(&(s -> maxlen))) = 10000;
  for (int i = 0; i < 100000; (*rts.avpush(&(i)))++) {
    event(s);
  }
  return 0;
}
