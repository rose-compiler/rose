#include "test-main.h"

const char* description = "Tests pointer-to-member call";
const char* expectedout = "{ABfa~B~A}";

struct A
{
  A()  { printf("A"); }
  ~A() { printf("~A"); }

  void out() const { printf("a"); }
};

typedef A Root;

struct B : Root
{
  B()  { printf("B"); }
  ~B() { printf("~B"); }

  void out() const { printf("b"); }
};


void foo(const B& obj, void (Root::*fn)() const)
{
  const A* a = &obj;

  printf("f");
  (a->*fn)();
}

void run()
{
  foo(B(), &A::out);
}
