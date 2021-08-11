#include "test-new.h"
#include "test-main.h"

const char* description = "Tests construction/destruction of dynamically allocated arrays and virtual destructors";
const char* expectedout = "{[gABABABABABABABABABABf~B~A~B~A~B~A~B~A~B~A~B~A~B~A~B~A~B~A~B~Ag]}";

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


void f(const A*) { printf("f"); }

void run()
{
  B* b = new B[10];

  f(b+6);

  delete[] b;
}

