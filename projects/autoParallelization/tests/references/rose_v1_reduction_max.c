#include "omp.h" 
double a[10];

int foo()
{
  double max_val = - 1e99;
  double min_val = 1e99;
  int i;
  
#pragma omp parallel for private (i) reduction (max:max_val) reduction (min:min_val)
  for (i = 0; i <= 9; i += 1) {
    if (a[i] > max_val) {
      max_val = a[i];
    }
    if (a[i] < min_val) 
      min_val = a[i];
  }
}
