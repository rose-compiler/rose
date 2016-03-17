/* 
 * Scalar-to-scalar dependencies
 * */
#include "omp.h" 
int a[100];

void foo2()
{
  int i;
  int tmp;
  
#pragma omp parallel for private (tmp,i)
  for (i = 0; i <= 99; i += 1) {
    tmp = a[i] + i;
    a[i] = tmp;
  }
}

void foo()
{
  int i;
  int tmp;
  
#pragma omp parallel for private (i) lastprivate (tmp)
  for (i = 0; i <= 99; i += 1) {
    tmp = a[i] + i;
    a[i] = tmp;
  }
  i = tmp;
}
