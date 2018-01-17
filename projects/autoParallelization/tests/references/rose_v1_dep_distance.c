/*
 *Test dependence distance 
 * */
#include "omp.h" 

void foo()
{
  int i;
  int a[100];
/* Constant offset*/
  for (i = 0; i <= 98; i += 1) {
    a[i + 3] = a[i - 5] + 1;
  }
}

void foo2(int j,int k)
{
  int i;
  int a[100];
/*variable offset*/
  for (i = 0; i <= 98; i += 1) {
    a[i + j] = a[i + k] + 1;
  }
}
int b[100][100];

void foo3()
{
  int i;
  int j;
/*two level with constant offset*/
  
#pragma omp parallel for private (i,j)
  for (i = 1; i <= 99; i += 1) {
    for (j = 1; j <= 99; j += 1) {
      b[i][j] = b[i][j - 1] + 1;
    }
  }
}
