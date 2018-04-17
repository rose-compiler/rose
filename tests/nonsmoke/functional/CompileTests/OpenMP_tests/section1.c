#include <omp.h>
#include <stdio.h>
int main()
{
#pragma omp parallel
  {
#pragma omp single
    printf ("This is from the single directive\n");
#pragma omp sections nowait
    {
#pragma omp section
      {
        printf("hello from section 1\n");
      }
#pragma omp section
      {
        printf("hello from section 2\n");
      }
#pragma omp section
      {
        printf("hello from section 3\n");
      }
    } /* end of sections */
  }/* end of parallel */
  return 0;
}

