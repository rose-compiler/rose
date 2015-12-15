#include<omp.h>
#include <stdio.h>

void paroutput(char* s)
{
}

int main(void)
{
#ifdef _OPENMP
  omp_set_nested(1);
#endif

#pragma omp parallel num_threads(4)
  { 
#pragma omp parallel num_threads(4)
    printf("before single.\n");

#pragma omp single
    {

#pragma omp parallel num_threads(4)
      printf("Inside single.\n");
    }
#pragma omp parallel num_threads(4)
    printf("after single.\n");
  }
  return 0;
}


