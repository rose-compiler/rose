// t0540.cc
// overload resolution of pointer-to-member

struct A {
  int f();
  int f(int);
};

void g(int (A::*)(int));

void foo()
{
  g(&A::f);
}
