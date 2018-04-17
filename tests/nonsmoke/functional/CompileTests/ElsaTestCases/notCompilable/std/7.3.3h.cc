// 7.3.3h.cc

// turn on overloading
int dummy();             // line 4
void ddummy() { __testOverload(dummy(), 4); }

asm("collectLookupResults f=18 g=19 x=10");

namespace A {
  int x;                        // line 10
}

namespace B {
  int i;
  struct g { };
  struct x { };
  void f(int);
  void f(double);               // line 18
  void g(char);                 // OK: hides struct g   (line 19)
}

void func()
{
  int i;
  //ERROR(1): using B::i;       // error: i declared twice
  void f(char);
  using B::f;                   // OK: each f is a function
  f(3.5);                       // calls B::f(double)
  using B::g;
  g('a');                       // calls B::g(char)
  struct g g1;                  // g1 has class type B::g
  using B::x;
  using A::x;                   // OK: hides struct B::x
  x = 99;                       // assigns to A::x
  struct x x1;                  // x1 has class type B::x
}

