#include "test-main.h"

const char* description = "Tests non-virtual call of a virtual function";
const char* expectedout = "{ABfa~B~A}";

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

void foo(const A& obj) { printf("f"); obj.A::out(); }

void run()
{
  foo(B());
}

