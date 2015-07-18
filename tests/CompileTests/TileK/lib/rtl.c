
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"
#include "KLT/RTL/build-loop-context.h"

#ifdef TILEK_THREADS
void tilek_fork_thread(int tid, kernel_func_ptr kernel_ptr, int * param, void ** data, void ** scalar, struct klt_loop_context_t * context);
void tilek_join_threads(int num_threads);
#endif

struct kernel_t * build_kernel(int idx) {
  struct kernel_desc_t * desc = &(kernel_desc[idx]);

  int size = sizeof(struct kernel_t)
           + desc->num_data   * sizeof(void *)
           + desc->num_param  * sizeof(int)
           + desc->num_scalar * sizeof(void *)
           + desc->num_loops  * sizeof(struct klt_loop_t);

  void * alloc = malloc(size);

  memset(alloc, 0, size);

  struct kernel_t * res = (struct kernel_t *)alloc;
      res->desc   = desc;
      res->data   =             (void **)(alloc += sizeof(struct kernel_t));
      res->param  =               (int *)(alloc += desc->num_data   * sizeof(void *));
      res->scalar =             (void **)(alloc += desc->num_param  * sizeof(int));
      res->loops  = (struct klt_loop_t *)(alloc += desc->num_scalar * sizeof(void *));

  return res;
}

void execute_kernel(struct kernel_t * kernel) {
  struct klt_loop_context_t * context = klt_build_loop_context(kernel->desc->num_loops, kernel->desc->num_tiles, kernel->desc->loop_desc, kernel->loops, kernel);

#if TILEK_THREADS
  int tid;
  // Fork slave threads
  for (tid = 1; tid < kernel->num_threads; tid++)
    tilek_fork_thread(tid, kernel->desc->kernel_ptr, kernel->param, kernel->data, kernel->scalar, &context);
  // Execute on master thread
  (*kernel->desc->kernel_ptr)(0, kernel->param, kernel->data, kernel->scalar, context);
  // Join thread
  tilek_join_threads(kernel->num_threads);
#else
  (*kernel->desc->kernel_ptr)(kernel->param, kernel->data, kernel->scalar, context);
#endif
}

int get_length_tile(struct kernel_t * kernel, unsigned long kind) {
#ifdef TILEK_THREADS
  assert(kind == 2);
  assert(kernel->num_threads > 0);
  return kernel->num_threads;
#else
  assert(0);
#endif
}

#ifdef TILEK_THREADS
void tilek_fork_thread(int tid, kernel_func_ptr kernel_ptr, int * param, void ** data, void ** scalar, struct klt_loop_context_t * context) {
  // TODO
}

void tilek_join_threads(int num_threads) {
  // TODO
}
#endif

