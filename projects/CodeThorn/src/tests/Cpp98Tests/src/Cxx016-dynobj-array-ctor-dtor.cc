#include "test-new.h"
#include "test-main.h"

const char* description = "Tests construction/destruction of dynamically allocated arrays";
const char* expectedout = "{[gAAAAAAAAAAfy~A~A~A~A~A~A~A~A~A~Ag]}";

struct A
{
  A()
  : cx('x'), cy('y'), cz('z')
  {
    printf("A");
  }

  ~A() { printf("~A"); }

  char cx;
  char cy;
  char cz;
};


void f(const A* a) { printf("f%c", a->cy); }

void run()
{
  A* a = new A[10];

  f(a+3);

  delete[] a;
}

