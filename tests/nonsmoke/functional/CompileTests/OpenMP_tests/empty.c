/*
1. how to include header
2. parallel region
3. runtime routine
4. undeterminated execution order
5. control number of threads

By C. Liao
*/
#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int main(void)
{
  int i=0;

#pragma omp parallel 
  {
#ifdef _OPENMP
    i=omp_get_thread_num();
#endif
    printf("Hello,world! I am thread %d\n",i);
  }
  return 0;
}

