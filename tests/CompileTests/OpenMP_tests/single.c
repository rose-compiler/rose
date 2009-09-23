/*
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#include<assert.h> 

int main(void)
{
  int i = 100  ;
  int num_threads =0;

#pragma omp parallel
  {

#pragma omp single
    {
      num_threads = omp_get_num_threads();
#pragma omp atomic
      i+=100;
    }

#pragma omp single nowait
    {
      num_threads = omp_get_num_threads();
    }

  }
  assert(i == 200);
  return 0;
}

