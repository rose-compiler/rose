#include "upc_strict.h"

relaxed shared int array1[100*THREADS];
strict shared int array2[100*THREADS];

int main()
{
  
#pragma upc strict
  
  return 0;  
}
