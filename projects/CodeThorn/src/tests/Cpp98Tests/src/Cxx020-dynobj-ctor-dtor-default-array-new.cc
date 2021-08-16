//~ #include "test-new.h" : use the built-in new and delete operator
#include "test-main.h"

const char* description = "Tests construction/destruction of dynamically allocated objects using the built-in new[] and delete[] operators."
                          "\nNote, the built-in new[] and delete operators[] produce no trace.";
const char* expectedout = "{AAAfy~A~A~A}";

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
  A* a = new A[3];

  f(a+1);

  delete[] a;
}

