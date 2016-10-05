// t0188.cc
// Assertion failed: asked for list element 0 (0-based) but list only
// has 0 elements, file voidlist.cc line 43
//
// cause: instantating a template whose definition scope is no longer
// on the scope stack

namespace N {
  template <class T>
  struct C {
    T *t;
  };
}

int foo()
{
  N::C<int> c;
  return *(c.t);
}
