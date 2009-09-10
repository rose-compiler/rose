#include <omp.h>

int main (void)
{

  int a;
#pragma omp parallel 
  {
#pragma omp atomic
    a+=1;
  }

}
