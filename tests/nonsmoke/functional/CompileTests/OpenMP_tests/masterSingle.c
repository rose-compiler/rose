#include <omp.h>

int main (void)
{

  int a,b;
#pragma omp parallel 
  {
#pragma omp master
    a=a+1;
#pragma omp single
    b=b+1;
  }
  return 0;
}
