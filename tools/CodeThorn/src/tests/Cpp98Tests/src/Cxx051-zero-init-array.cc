#include "test-new.h"
#include "test-main.h"

const char* description = "Tests default arguments for ctors";
const char* expectedout = "{[g0g]}";

// see also https://stackoverflow.com/questions/29765961/default-value-and-zero-initialization-mess

void run()
{
  int* a = new int[123]();

  printf("%c", '0'+ a[87]);
  delete[] a;
}

