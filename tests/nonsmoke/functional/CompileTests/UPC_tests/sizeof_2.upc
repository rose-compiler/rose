#include <stdio.h>
#include "upc.h"

shared[5] double a[100*THREADS]; 

int main()
{
  printf ("size of a is:%d\n",sizeof(a));
  printf ("local size of a is:%d\n",upc_localsizeof(a));
  printf ("block size of a is:%d\n",upc_blocksizeof(a));
  printf ("element size of a is:%d\n",upc_elemsizeof(a));
  return 0;
}
