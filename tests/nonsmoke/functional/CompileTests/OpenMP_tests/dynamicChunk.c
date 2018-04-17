#include <stdio.h>
#include <omp.h>
int foo (void)
{
  double a[1000];
  int i;
  int n;
  scanf("%d",&n);
#pragma omp for schedule(dynamic,50)
  for (i=0;i<n;i++)
  {
    a[i]=(double)i/2.0;
  }
  printf("a[878]=%f\n",a[878]);

  return 0;
}

