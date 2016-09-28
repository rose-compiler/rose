// t0160.cc
// simple use of a "using declaration"

namespace N {
  int x;
}

int foo()
{
  using N::x;
  return x;
}

