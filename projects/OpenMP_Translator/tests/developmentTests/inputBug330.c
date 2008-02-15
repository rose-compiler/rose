/*
test preprocessing info before and after a statement
*/
#include <stdio.h>
#ifdef _OPENMP
#include "omp.h"
#endif

int main()
{
int nthreads;
#pragma omp parallel 
  {
nthreads++;
#if defined(_OPENMP)
  #pragma omp master
    {
     printf("I am the master thread.\n"); 
    }
#endif
//  nthreads++; // things are quite different if this stmt exists!!
  }
  return 0;
}
