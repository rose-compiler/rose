#include "test-main.h"

const char* description = "Tests construction/destruction sequence of a temporary object with lifetime extension";
const char* expectedout = "{fA;~A}";

struct A
{
  A()         { printf("A");  }
  ~A()        { printf("~A"); }
};

A foo() { printf("f"); return A(); }

void run()
{
  const A& a = foo();

  printf(";");
}
