
#include "RTL/Host/tilek-rtl.h"

#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/data.h"
#include "KLT/RTL/context.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

struct klt_user_config_t * klt_user_build_config(struct klt_kernel_desc_t * desc)  {
  return malloc(sizeof(struct klt_user_config_t));
}

void klt_user_schedule(
  struct klt_kernel_t * kernel, struct klt_subkernel_desc_t * subkernel,
  struct klt_loop_context_t * klt_loop_context, struct klt_data_context_t * klt_data_context
) {
  int i;

  void ** local_param = (void **)malloc(subkernel->num_params * sizeof(void *));
  for (i = 0; i < subkernel->num_params; i++) {
    local_param[i] = kernel->param[subkernel->param_ids[i]];
  }

  void ** local_data = (void **)malloc(subkernel->num_data * sizeof(void *));
  for (i = 0; i < subkernel->num_data; i++) {
    local_data[i] = kernel->data[subkernel->data_ids[i]].ptr;
  }

  (*subkernel->config->kernel_ptr)(local_param, local_data, klt_loop_context, klt_data_context);
}

void klt_user_wait(struct klt_kernel_t * kernel) { /* NOP */ }

int klt_user_get_tile_length(struct klt_kernel_t * kernel, unsigned long kind, unsigned long param) {
  assert(0);
}

