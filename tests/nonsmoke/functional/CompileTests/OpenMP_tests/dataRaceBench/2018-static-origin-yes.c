// Declared in a scope inside the construct: private by default
// but if static storage: it is shared
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char* argv[])
{
  int i;
  int len=100;
  int a[100];

  for (i=0;i<len;i++)
    a[i]=i;

#pragma omp parallel for
  for (i=0;i<len;i++)
  {
    static int tmp = 0;
    tmp = a[i]+i;
    a[i] = tmp;
  }

  printf("a[50]=%d\n", a[50]);
  return 0;
}
