#include "test-new.h"
#include "test-main.h"

const char* description = "Tests default arguments for ctors";
const char* expectedout = "{gXg}";

// see also https://stackoverflow.com/questions/29765961/default-value-and-zero-initialization-mess

void run()
{
  char* a = new char[34]('X');

  printf("%c", '0'+ a[29]);
  delete[] a;
}

