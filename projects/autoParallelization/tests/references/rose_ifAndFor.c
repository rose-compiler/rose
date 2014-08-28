//! if statement without an explicit basic block as a body
#include "omp.h" 

void foo(int j)
{
  int i;
  int a[100];
  if (j != - 1) {
    
#pragma omp parallel for private (i)
    for (i = 0; i <= 99; i += 1) {
      a[i] = a[i] + 1;
    }
  }
}
