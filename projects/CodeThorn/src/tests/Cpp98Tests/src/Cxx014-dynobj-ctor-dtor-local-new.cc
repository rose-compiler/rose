#include "test-new.h"
#include "test-main.h"

const char* description = "Tests construction/destruction of dynamically allocated objects and overloaded new operator";
const char* expectedout = "{(AAB~B~AA)}";

struct A
{
  A()  { printf("A"); }
  virtual ~A() { printf("~A"); }

  void* operator new(std::size_t n) throw(std::bad_alloc)
  {
    printf("(A");
    return malloc(n);
  }

  void operator delete(void* p) throw()
  {
    printf("A)");
    free(p);
  }
};

struct B : A
{
  B()  { printf("B"); }
  ~B() { printf("~B"); }
};


void run()
{
  A* a = new B;

  delete a;
}

