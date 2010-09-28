/*
variable references within OpenMP pragma
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

void foo()
{
  float x;
#pragma omp parallel private(x)
  {
     x=10;
  }
}

