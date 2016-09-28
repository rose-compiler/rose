// 7.3.3e.cc

asm("collectLookupResults f=5 g=8");

void f();            // line 5

namespace A {
  void g();          // line 8
}

namespace X {
  using ::f;         // global f
  using A::g;        // A's g
}

void h()
{
  X::f();            // calls ::f
  X::g();            // calls A::g
}
