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

#pragma omp parallel
  {
#pragma omp parallel
    printf("before single.\n");

#pragma omp single
    {

#pragma omp parallel
      printf("Inside single.\n");
    }
#pragma omp parallel
    printf("after single.\n");
  }
}


