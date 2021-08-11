#include "test-new.h"
#include "test-main.h"

const char* description = "Tests construction/destruction of dynamically allocated objects using placement new."
                          "\nNote, placement new cannot be overloaded, thus there will be no tracing output for that call.";
const char* expectedout = "{ACfy~A}";

struct A
{
  A()
  : cx('x'), cy('y'), cz('z')
  {
    printf("A");
  }

  ~A() { printf("~A"); }

  int cx;
  int cy;
  int cz;
};


void check_location(A* obj, char* p, size_t len)
{
  bool withinBounds =  (  (static_cast<void*>(obj) >= static_cast<void*>(p))
                       && (static_cast<void*>(obj+1) < static_cast<void*>(p+len))
                       );

  printf(withinBounds ? "C" : "F");
}

void f(A* obj)
{
  printf("f%c", obj->cy);
}


void run()
{
  char storage[2*sizeof(A)];

  // placement new cannot be overloaded, so there is no debug output
  A* a = new (&storage) A;

  check_location(a, storage+0, sizeof(storage));
  f(a);

  a->~A();
}

