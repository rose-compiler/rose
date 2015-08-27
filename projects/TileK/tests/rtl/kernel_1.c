
#include "KLT/RTL/context.h"

void kernel_k0_v0_0(void ** param, void ** data, struct klt_loop_context_t * loop_ctx, struct klt_data_context_t * data_ctx) {
  int n = *(int *)param[0];
  float b = *(float *)param[1];
  float * A = (float *)data[0];

  int t_0;
  int l_0;
  for (t_0 = 0; t_0 < klt_get_tile_length(loop_ctx, 0); t_0 += klt_get_tile_stride(loop_ctx, 0)) {
    l_0 = t_0;
    A[l_0] += b;
  }
}

