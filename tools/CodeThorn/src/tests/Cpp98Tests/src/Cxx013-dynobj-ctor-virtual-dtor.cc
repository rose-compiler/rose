#include "test-new.h"
#include "test-main.h"

const char* description = "Tests construction/destruction of dynamically allocated objects with a virtual destructor";
const char* expectedout = "{(gABf~B~Ag)}";

struct A
{
  A()  { printf("A"); }
  virtual ~A() { printf("~A"); }
};

struct B : A
{
  B()  { printf("B"); }
  ~B() { printf("~B"); }
};


void f(A*)
{
  printf("f");
}

void run()
{
  A* a = new B;

  f(a);

  delete a;
}

