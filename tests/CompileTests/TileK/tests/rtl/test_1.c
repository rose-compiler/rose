
#include "RTL/Host/klt-user.h"

#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/data.h"

#include <stdlib.h>

int main() {
  int i;

  int n = 16;
  float b = 3.5;

  float * A = malloc(n * sizeof(float));
  for (i = 0; i < n; i++)
    A[i] = i;

  struct klt_kernel_t * kernel = klt_build_kernel(0);

  kernel->param[0] = &n;
  kernel->param[1] = &b;

  kernel->data[0].ptr = &A[0];

  kernel->loops[0].lower = 0;
  kernel->loops[0].upper = n-1;
  kernel->loops[0].stride = 1;

  klt_execute_kernel(kernel);

  return 0;
}

