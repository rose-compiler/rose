/*
1. manual control over work sharing
*/
#ifdef _OPENMP
#include <omp.h>
#endif 
#include <stdio.h>

#define N 20
int a[N];
int main(void)
{
  int i;
  int id, Nthrds, istart, iend;
#pragma omp parallel private(i,id,Nthrds,istart,iend)
  {
    /* each thread get self ID and total number of threads
       then decide which portion of work to do
       */
    id = omp_get_thread_num();
    Nthrds = omp_get_num_threads();
    istart= id* N/ Nthrds;
    iend = (id+1)*N/Nthrds; 

    //  for (i=0;i<20;i++)
    for (i=istart;i<iend;i++)
    {
      a[i]=i*2;
      printf("Iteration %2d is carried out by thread %2d\n",\
          i, omp_get_thread_num());
    }
  }
  return 0;
}

