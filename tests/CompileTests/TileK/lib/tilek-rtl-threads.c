
#include "RTL/Host/tilek-rtl.h"

#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/data.h"
#include "KLT/RTL/context.h"

#include <pthread.h>

#include <stdlib.h>
#include <string.h>

#include <assert.h>

struct klt_user_config_t * klt_user_build_config(struct klt_kernel_desc_t * desc)  {
  struct klt_user_config_t * config = malloc(sizeof(struct klt_user_config_t));
    config->num_threads = 1;
  return config;
}

struct klt_version_desc_t * klt_user_select_kernel_version(struct klt_kernel_t * kernel) {
  assert(kernel->desc->num_versions == 1);
  return &(kernel->desc->versions[0]);
}

struct tilek_worker_args_t {
  int tid;
  struct klt_kernel_t * kernel;
  struct klt_subkernel_desc_t * subkernel;
  void ** local_param;
  void ** local_data;
  struct klt_loop_context_t * klt_loop_context;
  struct klt_data_context_t * klt_data_context;
};

void * tilek_worker(void * args_) {
  struct tilek_worker_args_t * args = (struct tilek_worker_args_t *)args_;

  (*args->subkernel->config->kernel_ptr)(args->tid, args->local_param, args->local_data, args->klt_loop_context, args->klt_data_context);

  pthread_exit(NULL);
}

size_t num_threads;
pthread_t * threads;
struct tilek_worker_args_t * threads_args;

void klt_user_schedule(
  struct klt_kernel_t * kernel, struct klt_subkernel_desc_t * subkernel,
  struct klt_loop_context_t * klt_loop_context, struct klt_data_context_t * klt_data_context
) {
  assert(num_threads == 0);
  assert(threads == 0);
  assert(threads_args == 0);

  void ** local_param = NULL;
  void ** local_data = NULL;
  {
    int i;
    local_param = (void **)malloc(subkernel->num_params * sizeof(void *));
    local_data = (void **)malloc(subkernel->num_data * sizeof(void *));
    for (i = 0; i < subkernel->num_params; i++)
      local_param[i] = kernel->param[subkernel->param_ids[i]];
    for (i = 0; i < subkernel->num_data; i++)
      local_data[i] = kernel->data[subkernel->data_ids[i]].ptr;
  }

  num_threads = kernel->config->num_threads;
  threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
  threads_args = (struct tilek_worker_args_t *)malloc(num_threads * sizeof(struct tilek_worker_args_t));

  pthread_attr_t threads_attr;
  pthread_attr_init(&threads_attr);
  pthread_attr_setdetachstate(&threads_attr, PTHREAD_CREATE_JOINABLE);

  void * status;
  int rc;
  int tid;

  for (tid = 0; tid < num_threads; tid++) {
    threads_args[tid].tid = tid;
    threads_args[tid].kernel = kernel;
    threads_args[tid].subkernel = subkernel;
    threads_args[tid].local_param = local_param;
    threads_args[tid].local_data = local_data;
    threads_args[tid].klt_loop_context = klt_loop_context;
    threads_args[tid].klt_data_context = klt_data_context;
    
    rc = pthread_create(&threads[tid], &threads_attr, tilek_worker, &threads_args[tid]);
    assert(!rc);
  }

  pthread_attr_destroy(&threads_attr);
}

void klt_user_wait(struct klt_kernel_t * kernel) {
  void * status;
  int rc;
  int tid;

  for (tid = 0; tid < num_threads; tid++) {    
    rc = pthread_join(threads[tid], &status);
    assert(!rc);
  }

  free(threads_args); threads_args = 0;
  free(threads); threads = 0;
  num_threads = 0;
}

int get_length_tile(struct klt_kernel_t * kernel, unsigned long kind) {
  return kernel->config->num_threads;
}

