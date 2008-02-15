// t0157.cc
// very simple namespace example

asm("collectLookupResults x=7");

namespace N {
  int x;               // line 7
}

int f()
{
  return N::x;
}
