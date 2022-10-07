#include "test-new.h"
#include "test-main.h"

const char* description = "Tests construction/destruction of dynamically allocated arrays and virtual destructors."
                          "\nNote, this is UNDEFINED BEHAVIOR since the static and the dynamic type of a differ in the call to delete[]."
                          "\n5.3.5/3 C++03";
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
  A* a = new B[10];

  f(a+3);

  delete[] a;
}

