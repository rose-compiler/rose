// t0382.cc
// use a 'using' declaration to name an enum

// icc fails to reject: 1

namespace N {
  enum E { e };
  int g(int);
}

struct S { int x; };

void foo()
{
  using namespace N;     // get it all

  enum E e1;
  struct S s1;

  //ERROR(1): enum S s2;      // S is a struct, not an enum

  int x = e;      // 'e' *does* come along

  g(3);
}


void bar()
{
  using N::E;     // get just 'E'

  enum E e1;
  struct S s1;

  //ERROR(2): int x = e;      // 'e' does not come along

  int y = N::e;   // must explicitly qualify
}
