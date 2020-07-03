// test varRef in a function using variable arguments
#include <stdio.h>
void foo()
{
  int i=1,j=100;
#pragma rose_outline
  {
    printf ("3 arguments for 2 arguments prototype:%d, %d \n", i,j);
  }

}
