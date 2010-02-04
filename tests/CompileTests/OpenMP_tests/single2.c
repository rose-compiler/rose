/*
 * a pragma immediately following another 
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 

void foo(void)
{
  int num_threads =0;

#pragma omp parallel
#pragma omp single
  num_threads = omp_get_num_threads();
}

