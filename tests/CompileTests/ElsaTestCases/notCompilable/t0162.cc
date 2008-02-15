// t0162.cc
// simple using directive

asm("collectLookupResults x=7");

namespace N {
  int x;           // line 7
}

using namespace N;

int foo()
{
  return x;        // N::x
}
