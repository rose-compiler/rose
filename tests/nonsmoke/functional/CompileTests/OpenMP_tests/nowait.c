/*
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
int a[20];
void foo()
{
  int i;
#pragma omp for nowait
  for (i=0;i<20;i++)
  {
    a[i]=i*2;
  }
}

