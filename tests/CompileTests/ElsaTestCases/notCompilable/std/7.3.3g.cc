// 7.3.3g.cc

// turn on overloading
int dummy();             // line 4
void ddummy() { __testOverload(dummy(), 4); }

asm("collectLookupResults f=10 f=16");

namespace A {
  void f(int);          // line 10
}

using A::f;             // f is a synonym for A::f;
                        // that is, for A::f(int).
namespace A {
  void f(char);         // line 16
}

void foo()
{
  f('a');               // calls f(int),
}                       // even though f(char) exists.

void bar()
{
  using A::f;           // f is a synonym for A::f;
                        // that is, for A::f(int) and A::f(char).
  f('a');               // calls f(char)
}
