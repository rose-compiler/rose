// t0296.cc
// check receiver arg

struct A {
  int f();
  int g() const;
};

void foo(A const &a2)
{
  A a;
  a.f();                   // ok

  a2.g();                  // ok

  //ERROR(1): A::f();      // no recevier supplied

  //ERROR(2): a2.f();      // cannot convert
}
