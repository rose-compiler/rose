#include <omp.h>
#include <stdio.h>
int main(void)
{
  double a[1000];
  int i;
  int n;
  scanf("%d",&n);
#pragma omp for schedule(static)
  for (i=0;i<n;i++)
  {
    a[i]=(double)i/2.0;
  }
  printf("a[878]=%f\n",a[878]);

  return 0;
}

