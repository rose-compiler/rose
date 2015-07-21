
#include "RTL/Host/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"
#include "KLT/RTL/build-loop-context.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern void launch(struct kernel_t * kernel, struct klt_loop_context_t * klt_loop_context);

struct kernel_t * build_kernel(int idx) {
  struct kernel_desc_t * desc = &(kernel_desc[idx]);

  int size = sizeof(struct kernel_t)
           + desc->data.num_param  * sizeof(void *)
           + desc->data.num_scalar * sizeof(void *)
           + desc->data.num_data   * sizeof(void *)
           + desc->data.num_priv   * sizeof(void *)
           + desc->loop.num_loops  * sizeof(struct klt_loop_t);

  void * alloc = malloc(size);

  memset(alloc, 0, size);

  struct kernel_t * res = (struct kernel_t *)alloc;
      res->desc   = desc;
      res->param  =             (void **)(alloc += sizeof(struct kernel_t));
      res->scalar =             (void **)(alloc += desc->data.num_param  * sizeof(void *));
      res->data   =             (void **)(alloc += desc->data.num_scalar * sizeof(void *));
      res->priv   =             (void **)(alloc += desc->data.num_data   * sizeof(void *));
      res->loops  = (struct klt_loop_t *)(alloc += desc->data.num_priv   * sizeof(void *));

  return res;
}

void execute_kernel(struct kernel_t * kernel) {
  launch(kernel, klt_build_loop_context(&(kernel->desc->loop), kernel->loops, kernel));
}

