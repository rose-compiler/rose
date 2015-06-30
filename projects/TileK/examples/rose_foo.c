
#define TILEK_USER 1

#include "TileK/loop.h"
#include "TileK/kernel.h"

void foo(int n, int m, float ** A, float b) {
  struct kernel_t * kernel = build_kernel(0);

  kernel->data[0] = &(A[0][0]);
  kernel->data[1] = &(b);

  kernel->param[0] = n;
  kernel->param[1] = m;

  kernel->loops[0].lower = 0;
  kernel->loops[0].upper = n;
  kernel->loops[0].stride = 1;

  kernel->loops[1].lower = 0;
  kernel->loops[1].upper = m;
  kernel->loops[1].stride = 1;

  execute_kernel(kernel);
}

