/*
test fine grained locks instead of critical section
by Chunhua Liao
*/
#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#define LOCKNUM 100
#endif

#define SIZE 5000
int main(void)
{
  int a[SIZE];
  int i,j,sum,lock_index;

#ifdef _OPENMP
  omp_lock_t lck[LOCKNUM];
  for (i=0;i<LOCKNUM;i++)
    omp_init_lock(&(lck[i]));
#endif

  for (i=0;i<SIZE;i++)
    a[i]=0;

#pragma omp parallel private (i,j,lock_index)
  {

    /*critical version*/

#pragma omp for schedule(dynamic,1)
    for (i=0;i<SIZE;i++)
    {
      j=(i*i)%SIZE;
#pragma omp critical
      {
        a[j]=a[j]+5;
      }
    }

    /* fine grained lock version*/

#pragma omp for schedule(dynamic,1)
    for (i=0;i<SIZE;i++)
    {
      j=(i*i)%SIZE;
#ifdef _OPENMP
      lock_index= j%LOCKNUM; 
      //    omp_set_lock(lck[lock_index]);
#endif
      a[j]=a[j]-5;
#ifdef _OPENMP
      //    omp_unset_lock(lck[lock_index]);
#endif
    }

    /*verify the result*/
    sum=0;
#pragma omp for reduction (+:sum)
    for (i=0;i<SIZE;i++)
    {
      sum+=a[i];
    }

  }

  /* destroy locks*/
#ifdef _OPENMP
  for (i=0;i<LOCKNUM;i++)
    omp_destroy_lock(&(lck[i]));
#endif
  printf("sum of a[] = %d\n",sum);
  return 0;
}

