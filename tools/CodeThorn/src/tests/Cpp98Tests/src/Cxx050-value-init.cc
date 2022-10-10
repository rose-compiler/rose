#include <cassert>
#include "test-main.h"

const char* description = "Tests default arguments for ctors";
const char* expectedout = "{0}";

// see also https://stackoverflow.com/questions/29765961/default-value-and-zero-initialization-mess

struct A
{
  A(): x(0) { }

  int x;
};

void run()
{
  A* a = new A();

  printf("%c", '0'+a->x);
  delete a;
}

