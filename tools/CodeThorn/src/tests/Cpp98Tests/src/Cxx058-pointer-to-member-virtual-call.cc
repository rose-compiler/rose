#include "test-main.h"

const char* description = "Tests virtual pointer-to-member call";
const char* expectedout = "{ABfb~B~A}";

struct A
{
  A()  { printf("A"); }
  ~A() { printf("~A"); }

  virtual void out() const { printf("a"); }
};

struct B : A
{
  B()  { printf("B"); }
  ~B() { printf("~B"); }

  void out() const OVERRIDE { printf("b"); }
};

void foo(const A& obj, void (A::*fn)() const)
{
  printf("f");
  (obj.*fn)();
}

void run()
{
  foo(B(), &A::out);
}

