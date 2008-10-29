#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#define LOOPCOUNT 100

int main(void)
{
  int result = 0;
  int nr_iterations = 0;
  int i;
  int j;

#pragma omp parallel private(i,j)
  {
    for (i = 0; i < LOOPCOUNT; i++)
      {
#pragma omp single copyprivate(j)
        {
          nr_iterations++;
          j = i;
        }
        /*      #pragma omp barrier */
#pragma omp critical
        {
          result = result + j - i;
        }
#pragma omp barrier
      }                         /* end of for */

  }                             /* end of parallel */

 printf("result=%d nr_iterations=%d\n",result, nr_iterations);
  return (result == 0) && (nr_iterations == LOOPCOUNT);
}


