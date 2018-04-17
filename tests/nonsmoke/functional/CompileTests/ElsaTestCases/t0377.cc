// t0377.cc
// namespace-aware lookup yielding set

namespace N {
  void foo(int x);
}

using namespace N;

void foo(int x, int y);

void bar()
{
  foo(3);          // N::foo
  foo(4,5);        // ::foo
}
