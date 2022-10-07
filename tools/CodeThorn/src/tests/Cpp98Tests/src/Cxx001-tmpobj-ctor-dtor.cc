#include "test-main.h"

const char* description = "Tests construction/destruction sequence of a temporary object";
const char* expectedout = "{Af~A;}";

struct A
{
  A() { printf("A"); }
  ~A() { printf("~A"); }
};

const A& foo(const A& a) { printf("f"); return a; }

void run()
{
  const A& a = foo(A());

  // lifetime of A{} ends with the previous statement
  printf(";");
}
