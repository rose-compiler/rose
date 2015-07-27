
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

  struct kernel_t * res = (struct kernel_t *)malloc(sizeof(struct kernel_t));

  res->desc   = desc;

  res->param  = (void **)malloc(desc->data.num_param  * sizeof(void *));
  res->scalar = (void **)malloc(desc->data.num_scalar * sizeof(void *));

  res->data   = (struct klt_data_t *)malloc(desc->data.num_data * sizeof(struct klt_data_t));
  res->priv   = (struct klt_data_t *)malloc(desc->data.num_priv * sizeof(struct klt_data_t));

  res->loops  = (struct klt_loop_t *)malloc(desc->loop.num_loops * sizeof(struct klt_loop_t));

  int i;
  for (i = 0; i < desc->data.num_data; i++)
    res->data[i].sections = (struct klt_data_section_t *)malloc(desc->data.ndims_data[i] * sizeof(struct klt_data_section_t));
  for (i = 0; i < desc->data.num_priv; i++)
    res->priv[i].sections = (struct klt_data_section_t *)malloc(desc->data.ndims_priv[i] * sizeof(struct klt_data_section_t));

  return res;
}

void execute_kernel(struct kernel_t * kernel) {
  launch(kernel, klt_build_loop_context(&(kernel->desc->loop), kernel->loops, kernel));
}

