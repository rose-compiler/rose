
#include "RTL/Host/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"

#include <pthread.h>

#include <stdlib.h>
#include <string.h>

#include <assert.h>

struct tilek_worker_args_t {
  int tid;
  struct kernel_t * kernel;
  struct klt_loop_context_t * context;
};

void * tilek_worker(void * args) {
  struct tilek_worker_args_t * tilek_worker_args = (struct tilek_worker_args_t *)args;
  struct kernel_t * kernel = tilek_worker_args->kernel;

  int i;
  void ** local_data = (void **)malloc(kernel->desc->loop.num_loops * sizeof(void *));
  for (i = 0; i < kernel->desc->loop.num_loops; i++)
    local_data[i] = kernel->data[i].ptr;

  void ** local_private = NULL; // TODO

  (*kernel->desc->kernel_ptr)(tilek_worker_args->tid, kernel->param, kernel->scalar, local_data, local_private, tilek_worker_args->context);

  pthread_exit(NULL);
}

void launch(struct kernel_t * kernel, struct klt_loop_context_t * klt_loop_context) {
  void * status;
  int rc;

  int tid;
  pthread_t * threads = (pthread_t *)malloc(kernel->num_threads * sizeof(pthread_t));

  struct tilek_worker_args_t * threads_args = (struct tilek_worker_args_t *)malloc(kernel->num_threads * sizeof(struct tilek_worker_args_t));

  pthread_attr_t threads_attr;
  pthread_attr_init(&threads_attr);
  pthread_attr_setdetachstate(&threads_attr, PTHREAD_CREATE_JOINABLE);

  for (tid = 0; tid < kernel->num_threads; tid++) {
    threads_args[tid].tid     = tid;
    threads_args[tid].kernel  = kernel;
    threads_args[tid].context = klt_loop_context;
    
    rc = pthread_create(&threads[tid], &threads_attr, tilek_worker, &threads_args[tid]);
    assert(!rc);
  }

  pthread_attr_destroy(&threads_attr);

  for (tid = 0; tid < kernel->num_threads; tid++) {    
    rc = pthread_join(threads[tid], &status);
    assert(!rc);
  }
}

int get_length_tile(struct kernel_t * kernel, unsigned long kind) {
  assert(kind == 2);
  assert(kernel->num_threads > 0);
  return kernel->num_threads;
}

