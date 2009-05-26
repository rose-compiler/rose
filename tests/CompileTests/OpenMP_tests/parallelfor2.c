// I think this should be illegal?
#include <omp.h>
int main(void)
{
  int i, a[1000];
#pragma omp parallel if(1)
#pragma omp for
  for (i=0;i<1000;i++)
    a[i]=i*2;
return 1;
}
