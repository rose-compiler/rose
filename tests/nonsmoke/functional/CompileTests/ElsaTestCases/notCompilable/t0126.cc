// t0126.cc
// test overload resolution of member functions

asm("collectLookupResults f=7 a=13 f=8 a=13");

struct A {
  int f();        // line 7
  int f(int);     // line 8
};

void func()
{
  A a;            // line 13
  a.f();
  a.f(3);
}
