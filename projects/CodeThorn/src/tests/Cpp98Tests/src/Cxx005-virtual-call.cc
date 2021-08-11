#include "test-main.h"

const char* description = "Tests normal virtual function call";
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

void foo(const A& obj) { printf("f"); obj.out(); }

void run()
{
  foo(B());
}

