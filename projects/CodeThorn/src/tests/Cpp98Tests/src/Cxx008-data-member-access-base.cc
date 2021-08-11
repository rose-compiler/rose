#include "test-main.h"

const char* description = "Tests data member access in base class";
const char* expectedout = "{ABfa~B~A}";

struct A
{
  A() : v('a') { printf("A"); }
  ~A()         { printf("~A"); }

  virtual char val() const { return v; }

  char v;
};

struct B : A
{
  B() : v('b') { printf("B"); }
  ~B()         { printf("~B"); }

  virtual char val() const { return A::v; }

  char v;
};

void foo(const A& obj) { printf("f%c", obj.val()); }

void run()
{
  foo(B());
}

