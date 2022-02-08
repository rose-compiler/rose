#include "test-main.h"

const char* description = "Tests default parameters";
const char* expectedout = "{7}";

void foo(int val = 7)
{
  printf("%d", val);
}

void run()
{
  foo();
}

