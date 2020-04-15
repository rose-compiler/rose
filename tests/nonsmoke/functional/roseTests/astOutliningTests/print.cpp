// test varRef in a function using variable arguments
// j: index is out of bound for printf() with two arguments only
// i: index to the last argument ..., it should check for its only type
#include <stdio.h>
void foo(int & i)
{
  int j=100;
#pragma rose_outline
  {
    printf ("2 arguments for 2 arguments prototype:%d \n", i);
    printf ("2 arguments for 2 arguments prototype:%d \n", j);
    printf ("3 arguments for 2 arguments prototype:%d, %d \n", i,j);
  }

}
