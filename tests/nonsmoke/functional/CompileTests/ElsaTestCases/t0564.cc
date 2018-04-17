// t0564.cc
// causes ElaboratedOrSpecifier multi-yield

namespace N {
  template <class S>
  struct A {};
}

class B;

void foo()
{
  N::A<const class B> blah;
}
