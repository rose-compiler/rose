/*
test input
  a file without the main entry
By C. Liao
*/
#include <stdio.h>

#ifdef _OPENMP
#include "omp.h"
#endif

int foo(void)
{
#ifdef _OPENMP
  omp_set_nested(1);
#endif
#pragma omp parallel 
 printf("Hello,world!\n");

#pragma omp parallel
{
 printf("1Hello,world!\n");
#pragma omp parallel
 printf("2Hello,world!\n");
}

return 0;
}

