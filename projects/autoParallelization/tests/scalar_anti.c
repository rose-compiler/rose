/* 
 * Scalar-to-scalar dependencies
 * */
#include <stdio.h>
int a[100];
#if 1
void foo2()
{
  int i;
  int tmp;
  tmp = 10;
  // It would be wrong to parallelize the following loop
  // since the true dependence between tmp in an iteration
  // and tmp in the following iteration.
  // Even firstprivate cannot help this.
  for (i=0;i<100;i++)
  {
    a[i] = tmp;
    tmp =a[i]+i;
  }

  printf ("a[0]=%d\n", a[0]);
  printf ("a[20]=%d\n", a[20]);
  printf ("a[40]=%d\n", a[40]);
  printf ("a[60]=%d\n", a[60]);
  printf ("a[80]=%d\n", a[80]);
  printf ("a[99]=%d\n", a[99]);
}
#endif 

void foo()
{
  int i;
  int tmp;
  tmp = 10;
  // This should be parallelized using firstprivate
  for (i=0;i<100;i++)
  {
    a [i ] =tmp;
  }
  i = tmp; 
}

int main()
{
  foo2();
  return 0;
}
