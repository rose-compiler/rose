
#include "RTL/Host/kernel.h"

#include <assert.h>

int get_length_tile(struct kernel_t * kernel, unsigned long kind) {
  assert(kind > 1 && kind < 8);
  switch (kind) {
    case 2:
      assert(kernel->num_gangs[0] > 0);
      return kernel->num_gangs[0];
    case 3:
      assert(kernel->num_gangs[1] > 0);
      return kernel->num_gangs[1];
    case 4:
      assert(kernel->num_gangs[2] > 0);
      return kernel->num_gangs[2];
    case 5:
      assert(kernel->num_workers[0] > 0);
      return kernel->num_workers[0];
    case 6:
      assert(kernel->num_workers[1] > 0);
      return kernel->num_workers[1];
    case 7:
      assert(kernel->num_workers[2] > 0);
      return kernel->num_workers[2];
    default: assert(0);
  }
}
