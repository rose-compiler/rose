#include "test-main.h"

const char* description = "Tests subobject access using casts";
const char* expectedout = "{AE~A}";

struct A
{
  explicit
  A(const char* val) : data(val) { printf("A"); }

  virtual ~A() { printf("~A"); }

  const char* data;
};

struct B : A
{
  B() : A("ab"), data("b") { printf("B"); }
  ~B() { printf("~B"); }

  const char* data;
};

struct C
{
  C() : data("c") { printf("C"); }
  ~C() { printf("~C"); }

  const char* data;
};

struct D : C, B
{
  D() : C(), B(), data("d") { printf("D"); }
  ~D() { printf("~D"); }

  const char* data;
};

struct X : A
{
  X() : A("ax") {}
};


void f(const A& obj)
{
  printf("f%s", dynamic_cast<const D&>(obj).data);
}

void run()
{
  X x;

  try { f(x); } catch (...) { printf("E"); }
}
