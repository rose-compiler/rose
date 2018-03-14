/* 
 * Scalar-to-scalar output dependencies
 * */
#include "omp.h" 
int a[100];
// A private case

void foo2()
{
  int i;
  int tmp;
  
#pragma omp parallel for private (tmp,i)
  for (i = 0; i <= 99; i += 1) {
    tmp = a[i] + i;
  }
}
// A lastprivate case

void foo()
{
  int i;
  int tmp;
  
#pragma omp parallel for private (i) lastprivate (tmp)
  for (i = 0; i <= 99; i += 1) {
    tmp = a[i] + i;
  }
  i = tmp;
}
