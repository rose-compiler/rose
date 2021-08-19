#include "test-main.h"

const char* description = "Tests dtor calls with control-flow transfers";
const char* expectedout = "{AAB~B~A~AAAB~B~A~AA~A}";

struct A
{
  A()  { printf("A"); }
  ~A() { printf("~A"); }
};

struct B : A
{
  B()  { printf("B"); }
  ~B() { printf("~B"); }
};

void run()
{
  for (int i = 0; i < 3; ++i)
  {
    A a;

    if (i >= 2) return;

    B b;
  }
}

