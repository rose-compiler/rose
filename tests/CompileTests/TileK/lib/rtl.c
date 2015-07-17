
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/scheduler.h"

struct kernel_t * build_kernel(int idx) {
  struct kernel_desc_t * desc = &(kernel_desc[idx]);

  int size = sizeof(struct kernel_t)
           + desc->num_data   * sizeof(void *)
           + desc->num_param  * sizeof(int)
           + desc->num_scalar * sizeof(void *)
           + desc->num_loops  * sizeof(struct loop_t)
           + desc->num_tiles  * sizeof(struct tile_t);

  void * alloc = malloc(size);

  memset(alloc, 0, size);

  struct kernel_t * res = (struct kernel_t *)alloc;
      res->desc   = desc;
      res->data   =         (void **)(alloc += sizeof(struct kernel_t));
      res->param  =           (int *)(alloc += desc->num_data   * sizeof(void *));
      res->scalar =         (void **)(alloc += desc->num_param  * sizeof(int));
      res->loops  = (struct loop_t *)(alloc += desc->num_scalar * sizeof(void *));
      res->tiles  = (struct tile_t *)(alloc += desc->num_loops  * sizeof(struct loop_t));

  return res;
}

void execute_kernel(struct kernel_t * kernel) {
  klt_schedule(kernel->desc->num_loops, kernel->desc->loop_desc, kernel->loops, kernel->tiles);

  struct context_t context = { kernel->loops, kernel->tiles };
  (*kernel->desc->kernel_ptr)(kernel->param, kernel->data, kernel->scalar, &context);
}

