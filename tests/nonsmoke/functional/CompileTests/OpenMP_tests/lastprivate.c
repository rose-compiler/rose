#include <omp.h>
#define n 100
int a[n];
int main()
{
  int i,j;
  j = 0;
#pragma omp parallel for lastprivate(j)
  for(i=1; i<=n; i++){
      j = j + 1;
    a[i] = a[i] + j;
  }
  return 0;
}


