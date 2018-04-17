// t0413.cc
// variant of t0382.cc that also tests struct tags

namespace N {
  enum E { e };
  struct S { int x; };
  int g(int);
}

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
  using N::S;     // and 'S'

  enum E e1;
  struct S s1;

  //ERROR(2): int x = e;      // 'e' does not come along

  int y = N::e;   // must explicitly qualify
}
