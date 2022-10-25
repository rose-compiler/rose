#include "test-main.h"

const char* description = "Tests virtual function call in constructor/destructor";
const char* expectedout = "{AaB~B~Aa}";

struct A
{
  A()  { printf("A"); out(); }
  ~A() { printf("~A"); out(); }

  virtual void out() { printf("a"); }
};

struct B : A
{
  B()  { printf("B"); }
  ~B() { printf("~B"); }

  void out() { printf("b"); }
};

void run()
{
  B b;
}

