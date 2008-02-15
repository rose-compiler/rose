/*
test input

  handling of variable scopes:
   private, firstprivate, lastprivate, threadprivate
By C. Liao
*/
#include <stdio.h>

#ifdef _OPENMP
#include "omp.h"
#endif

void foo (double y[]){}

int gi = 0;
static int m = 1;
double mm[117];
double mm2[117];
#pragma omp threadprivate(m,mm)

int
main (void)
{
  int i;
  int k_3;
  int gj = 0;
  int j = 5;

  mm[0] = 9.0;
  k_3 = 7 + mm[0];

#pragma omp parallel private (i) firstprivate(k_3) reduction(+:gi,gj) copyin(mm)
  {
    int k = 1;
#ifdef  _OPENMP
    i = omp_get_thread_num ();
#endif
    printf ("Hello,world!\n");
#pragma omp critical
    gi = i + j + k + k_3;

    gi += i;
#pragma omp atomic
    j++;
    gj += m + mm[1];
    foo (mm - 1);
    foo (mm2 - 1);

  }
  printf ("gi is %d,gj is %d\n", gi, gj);
  return 0;
}
