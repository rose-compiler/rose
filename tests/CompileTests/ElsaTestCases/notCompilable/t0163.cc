// t0163.cc
// using directive example demonstrating difference between
// "using" and "active using" edges

asm("collectLookupResults g=13 x=8 g=13 x=26");

namespace N {
  int x;               // line 8
}

void foo()
{
  int g;               // line 13

  // This puts a "using" edge from foo's scope to N.  But that
  // is irrelevant because no one can nominate foo's scope using
  // another using-directive.  (So I will probably not add it
  // at all.)
  //
  // It also puts an "active using" edge from the global scope to N,
  // and schedules that edge for removal once foo's scope is gone.
  using namespace N;

  g = x;               // N::x

  int x;               // line 26
  g = x;               // local variable x, since N::x as if at toplevel
}

