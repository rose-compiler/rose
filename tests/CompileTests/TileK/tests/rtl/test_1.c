
#ifndef ORIGINAL
#define ORIGINAL 0
#endif
#if ORIGINAL == 0
#  include "RTL/Host/klt-user.h"
#  include "KLT/RTL/kernel.h"
#  include "KLT/RTL/loop.h"
#  include "KLT/RTL/tile.h"
#  include "KLT/RTL/data.h"
#endif

#include <stdlib.h>

int main() {
  int i;

  int n = 16;
  float b = 3.5;

  float * A = malloc(n * sizeof(float));
  for (i = 0; i < n; i++)
    A[i] = i;

#if ORIGINAL == 1
  #pragma kernel data(A[0:n])
  #pragma loop tile[0](dynamic)
  for (i = 0; i < n; i++)
    A[i] += b;
#else
  struct klt_kernel_t * kernel = klt_build_kernel(0);

  kernel->param[0] = &n;
  kernel->param[1] = &b;

  kernel->data[0].ptr = &A[0];
  kernel->data[0].sections[0].offset = 0;
  kernel->data[0].sections[0].length = n;

  kernel->loops[0].lower = 0;
  kernel->loops[0].upper = n-1;
  kernel->loops[0].stride = 1;

  klt_execute_kernel(kernel);
#endif

  return 0;
}

