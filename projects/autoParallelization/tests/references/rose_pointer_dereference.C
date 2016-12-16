#define max_ir 100
#include "omp.h" 
int *nd_array[100];
int *gr_array[100];

void foo(int rlenmix)
{
  int i;
  
#pragma omp parallel for private (i) firstprivate (rlenmix)
  for (i = 0; i <= rlenmix - 1; i += 1) {
    int nn =  *(nd_array[1] + i);
    int gg =  *(gr_array[1] + i);
    nn =  *(nd_array[2] + i);
    gg =  *(gr_array[2] + i);
  }
}
