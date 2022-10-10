#include "test-main.h"

const char* description = "Tests construction/destruction of arrays";
const char* expectedout = "{AAAAAAAAAAf~A~A~A~A~A~A~A~A~A~A}";

struct A
{
  A()  { printf("A"); }
  ~A() { printf("~A"); }
};


void f(const A*) { printf("f"); }

void run()
{
  A a[10];

  f(a+3);
}

