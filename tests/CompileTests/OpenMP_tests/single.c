/*
 loop scheduling
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 

int main(void)
{
  int i = 100;

#pragma omp parallel
  {
#pragma omp single firstprivate(i)
    {
      i += omp_get_num_threads();
      printf ("i= %d \n",i);
    }
  }
}

