/*
test input for regular parallel regions
  and nested parallel regions

By C. Liao
*/
#include <stdio.h>

#ifdef _OPENMP
#include "omp.h"
#endif

int
main (void)
{
#ifdef _OPENMP
  omp_set_nested (1);
#endif

/*1 level*/
#pragma omp parallel
  printf ("Hello,world!\n");


/*2 levels*/
#pragma omp parallel
  {
    printf ("1Hello,world!\n");
#pragma omp parallel
    printf ("2Hello,world!\n");
  }
  return 0;
}
