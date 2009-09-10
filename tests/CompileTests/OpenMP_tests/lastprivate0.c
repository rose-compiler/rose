#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
int main(void)
{
  int i,is=0;

#pragma omp parallel for private(is)
  for (i=0;i<100;i++)
    is = is+i;
  printf("%d=%d\n ",i,is);

  is=0;

#pragma omp parallel for firstprivate(is)
  for (i=0;i<100;i++)
    is = is+i;
  printf("%d=%d\n ",i,is);

  is=0;

#pragma omp parallel for lastprivate(is)
  for (i=0;i<100;i++)
    is = is+i;
  printf("%d=%d\n ",i,is);

  is=0;

  //#pragma omp parallel for lastprivate(is)
#pragma omp parallel for schedule(static,30) firstprivate(is) lastprivate(is)
  for (i=0;i<100;i++)
    is = is+i;

  /*The value of is depends on the num of threads and schedule method*/
  printf("%d, %d\n ",i,is);

  is=0;
  for (i=90;i<100;i++)
    is = is+i;
  printf("%d, %d\n ",i,is);
  return 0;
}
