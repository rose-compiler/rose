
#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"
#include "KLT/RTL/build-context.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern struct klt_user_config_t * klt_user_build_config(struct klt_kernel_desc_t * desc);

extern struct klt_version_desc_t * klt_user_select_kernel_version(struct klt_kernel_t * kernel);

extern void klt_user_schedule(
  struct klt_kernel_t * kernel, struct klt_subkernel_desc_t * subkernel,
  struct klt_loop_context_t * klt_loop_context, struct klt_data_context_t * klt_data_context
);

extern void klt_user_wait(struct klt_kernel_t * kernel);

struct klt_kernel_t * klt_build_kernel(int idx) {
  struct klt_kernel_desc_t * desc = &(klt_kernel_desc[idx]);

  struct klt_kernel_t * res = (struct klt_kernel_t *)malloc(sizeof(struct klt_kernel_t));

  res->desc   = desc;

  res->param  = (void **)malloc(desc->data.num_param  * sizeof(void *));

  res->data   = (struct klt_data_t *)malloc(desc->data.num_data * sizeof(struct klt_data_t));

  res->loops  = (struct klt_loop_t *)malloc(desc->loop.num_loops * sizeof(struct klt_loop_t));

  res->config = klt_user_build_config(desc);

  int i;
  for (i = 0; i < desc->data.num_data; i++)
    res->data[i].sections = (struct klt_data_section_t *)malloc(desc->data.ndims_data[i] * sizeof(struct klt_data_section_t));

  return res;
}

void klt_execute_kernel(struct klt_kernel_t * kernel) {
  struct klt_version_desc_t * version = klt_user_select_kernel_version(kernel);

  int i;
  for (i = 0; i < version->num_subkernels; i++) {
    struct klt_loop_context_t * klt_loop_context = klt_build_loop_context(&(version->subkernels[i].loop), kernel->loops, kernel);
    struct klt_data_context_t * klt_data_context = klt_build_data_context();
    klt_user_schedule(kernel, &(version->subkernels[i]), klt_loop_context, klt_data_context);
  }

  klt_user_wait(kernel);
}

