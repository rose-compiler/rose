#include "test-main.h"

const char* description = "Tests virtual function call through a pointer to member";
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

  void out() const { printf("b"); }
};

void foo(const A& obj, void(A::*fn)() const) { printf("f"); (obj.*fn)(); }

void run()
{
  foo(B(), &A::out);
}

