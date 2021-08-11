#include "test-new.h"
#include "test-main.h"

const char* description = "Tests construction/destruction of dynamically allocated objects";
const char* expectedout = "{(gAfy~Ag)}";

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


void f(A* obj)
{
  printf("f%c", obj->cy);
}

void run()
{
  A* a = new A;

  f(a);

  delete a;
}

