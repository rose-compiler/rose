
#include "RTL/Host/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"
#include "KLT/RTL/build-loop-context.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#if defined(TILEK_THREADS)
#include <pthread.h>

struct tilek_worker_args_t {
  int tid;
  kernel_func_ptr kernel_ptr;
  int   * param;
  void ** data;
  void ** scalar;
  struct klt_loop_context_t * context;
};

void * tilek_worker(void * args) {
  struct tilek_worker_args_t * tilek_worker_args = (struct tilek_worker_args_t *)args;

  (*tilek_worker_args->kernel_ptr)(tilek_worker_args->tid, tilek_worker_args->param, tilek_worker_args->data, tilek_worker_args->scalar, tilek_worker_args->context);

  pthread_exit(NULL);
}
#elif defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
// TODO
#  elif defined(TILEK_TARGET_CUDA)
// TODO
#  endif
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

#if defined(TILEK_THREADS)
  void * status;
  int rc;

  int tid;
  pthread_t * threads = (pthread_t *)malloc(kernel->num_threads * sizeof(pthread_t));

  struct tilek_worker_args_t * threads_args = (struct tilek_worker_args_t *)malloc(kernel->num_threads * sizeof(struct tilek_worker_args_t));

  pthread_attr_t threads_attr;
  pthread_attr_init(&threads_attr);
  pthread_attr_setdetachstate(&threads_attr, PTHREAD_CREATE_JOINABLE);

  for (tid = 0; tid < kernel->num_threads; tid++) {
    threads_args[tid].tid        = tid;
    threads_args[tid].kernel_ptr = kernel->desc->kernel_ptr;
    threads_args[tid].param      = kernel->param;
    threads_args[tid].data       = kernel->data;
    threads_args[tid].scalar     = kernel->scalar;
    threads_args[tid].context    = context;
    
    rc = pthread_create(&threads[tid], &threads_attr, tilek_worker, &threads_args[tid]);
    assert(!rc);
  }

  pthread_attr_destroy(&threads_attr);

  for (tid = 0; tid < kernel->num_threads; tid++) {    
    rc = pthread_join(threads[tid], &status);
    assert(!rc);
  }
#elif defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
  assert(0); // TODO
#  elif defined(TILEK_TARGET_CUDA)
  assert(0); // TODO
#  endif
#else
  (*kernel->desc->kernel_ptr)(kernel->param, kernel->data, kernel->scalar, context);
#endif
}

int get_length_tile(struct kernel_t * kernel, unsigned long kind) {
#if defined(TILEK_THREADS)
  assert(kind == 2);
  assert(kernel->num_threads > 0);
  return kernel->num_threads;
#elif defined(TILEK_ACCELERATOR)
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
#else
  assert(0);
#endif
}

