// 7.3.3i.cc

// turn on overloading
int dummy();             // line 4
void ddummy() { __testOverload(dummy(), 4); }

asm("collectLookupResults f=16");

namespace B {
  void f(int);
  void f(double);
}
namespace C {
  void f(int);
  void f(double);
  void f(char);               // line 16
}

void h()
{
  using B::f;                 // B::f(int) and B::f(double)
  using C::f;                 // C::f(int), C::f(double), and C::f(char)
  f('h');                     // calls C::f(char)
  //ERROR(1): f(1);           // error: ambiguous: B::f(int) or C::f(int)?
  //ERROR(2): void f(int);    // error:
                              // f(int) conflicts with C::f(int) and B::f(int)
}
