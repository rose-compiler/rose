/*
 *  Test for automatic recognition of reduction variables
 * */
#include "omp.h" 
int a[100];
int sum;

void foo()
{
  int i;
  int sum2;
  int xx;
  int yy;
  int zz;
  sum = 0;
  
#pragma omp parallel for private (i) reduction (+:sum,xx) reduction (-:yy) reduction (*:zz)
  for (i = 0; i <= 99; i += 1) {
    a[i] = i;
    sum = a[i] + sum;
    xx++;
    yy--;
    zz *= a[i];
  }
  sum2 = sum + xx + yy + zz;
  a[1] = 1;
}
