// t0557.cc
// default argument refers to namespace member

// actually, the default arg had nothing to do with it;
// it was an explicit inst request naming something in
// a namespace that caused the problem

namespace N {
  template <class T>
  struct A {};

  template <class U, class V = A<U> >
  struct B {};
}
using namespace N;

template struct B<int>;
