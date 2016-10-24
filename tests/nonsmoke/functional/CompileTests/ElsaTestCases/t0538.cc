// t0538.cc
// gotta push the semantic scope stack, not syntactic

namespace N {
  typedef int INT;
  struct A {
    void f();
  };
}

using N::A;

// the "N::" is not syntactally present, but must be searched anyway
void A::f()
{
  INT i;
}
