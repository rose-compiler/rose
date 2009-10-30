//#include <omp.h>
int j = 0;
void foo(int n)
{
  int i;
#pragma omp for lastprivate(j)
  for(i=1; i<n; i++){
      j = j + 1;
  }
}


