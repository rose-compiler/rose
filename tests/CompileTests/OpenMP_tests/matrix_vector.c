// double-precision floating-point matrix-vector product
// from BikEfficient2001
// segmentation fault ???
// May 30, 2008
#include <stdio.h>
#define SIZE 5000
int
main ()
{
  double d, a[SIZE][SIZE], x[SIZE], y[SIZE];
  int i,j,n;
  n=SIZE;
  for (i = 0; i < n; i++)
  {
    y[i]= (double)i*i; 
    for (j = 0; j < n; j++)
      a[i][j] = (double)(i/j);
  }        

  for (i = 0; i < n; i++)
  {
    d = 0.0;
    for (j = 0; j < n; j++)
    {
      d += a[i][j] * y[j];
    }
    x[i] = d;
  }
  printf("%f\n",x[0]);

  return 0;
}
