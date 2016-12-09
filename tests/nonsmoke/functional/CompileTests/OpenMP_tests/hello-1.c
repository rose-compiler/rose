/*
 * test the simplest case, no variable handling
By C. Liao
*/
#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int main(void)
{
#pragma omp parallel
  {
    printf("Hello,world!");
  }
  return 0;
}

