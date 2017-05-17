/*
 loop scheduling
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
int a[20];

void foo(int lower, int upper, int stride)
{
  int i;
#pragma omp single
printf("---------default schedule--------------\n");  
#pragma omp for  nowait
  for (i=lower;i<upper;i+=stride)
  {
    a[i]=i*2;
    printf("Iteration %2d is carried out by thread %2d\n",\
        i, omp_get_thread_num());
  }

#pragma omp barrier
#pragma omp single
printf("---------static schedule--------------\n");  
#pragma omp for schedule(static)
  for (i=lower;i<upper;i+=stride)
  {
    a[i]=i*2;
    printf("Iteration %2d is carried out by thread %2d\n",\
        i, omp_get_thread_num());
  }

#pragma omp single
printf("---------(static,5) schedule--------------\n");  
#pragma omp for schedule(static,5)
  for (i=lower;i<upper;i+=stride)
  {
    a[i]=i*2;
    printf("Iteration %2d is carried out by thread %2d\n",\
        i, omp_get_thread_num());
  }


#pragma omp single
printf("---------(dynamic,3) schedule--------------\n");  
#pragma omp for schedule(dynamic,3)
  for (i=lower;i<upper;i+=stride)
  {
    a[i]=i*2;
    printf("Iteration %2d is carried out by thread %2d\n",\
        i, omp_get_thread_num());
  }

#if 1
#pragma omp single
printf("---------(guided) schedule--------------\n");  
#pragma omp for schedule(guided) 
  for (i=lower;i<upper;i+=stride)
  {
    a[i]=i*2;
    printf("Iteration %2d is carried out by thread %2d\n",\
        i, omp_get_thread_num());
  }
#endif
#pragma omp single
printf("---------(runtime) ordered schedule--------------\n");  
#pragma omp for schedule(runtime) ordered
  for (i=lower;i<upper;i+=stride)
  {
    a[i]=i*2;
    printf("Iteration %2d is carried out by thread %2d\n",\
        i, omp_get_thread_num());
  }

}

int main(void)
{
  //#pragma omp parallel for schedule (auto)
#pragma omp parallel
  {
#pragma omp single
    printf ("Using %d threads.\n",omp_get_num_threads());
   foo(0,20,3);
  }
  return 0;
}

