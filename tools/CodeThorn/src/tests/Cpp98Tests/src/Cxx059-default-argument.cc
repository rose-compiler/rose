#include "test-main.h"

const char* description = "Tests default parameters";
const char* expectedout = "{A3~A}";

static const int X = 3;

struct A
{
  A(int i = X)  { printf("A%d", i); }
  ~A() { printf("~A"); }
};

void run()
{
  A a;
}

