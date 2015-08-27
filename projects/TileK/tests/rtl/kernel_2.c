
#include "KLT/RTL/context.h"

void kernel_k0_v0_0(void ** param, void ** data, struct klt_loop_context_t * loop_ctx, struct klt_data_context_t * data_ctx) {
  int n = *(int *)param[0];
  int p = *(int *)param[1];
  int q = *(int *)param[2];
  float alpha = *(float *)param[3];

  float * A = (float *)data[0];
  float * B = (float *)data[1];
  float * D = (float *)data[2];

  int t_0;
  int t_1;
  int l_0;
  int l_1;
  int k;
  for (t_0 = 0; t_0 < klt_get_tile_length(loop_ctx, 0); t_0 += klt_get_tile_stride(loop_ctx, 0)) {
    for (t_1 = 0; t_1 < klt_get_tile_length(loop_ctx, 1); t_1 += klt_get_tile_stride(loop_ctx, 1)) {
      l_0 = t_0;
      l_1 = t_1;
      D[l_0 * q + l_1] = 0;
      for (k = 0; k < p; k++) {
        D[l_0 * q + l_1] = A[l_0 * p + k] * B[k * q + l_1];
      }
      D[l_0 * q + l_1] *= alpha;
    }
  }
}

void kernel_k0_v0_1(void ** param, void ** data, struct klt_loop_context_t * loop_ctx, struct klt_data_context_t * data_ctx) {
  int n = *(int *)param[0];
  int m = *(int *)param[1];
  int q = *(int *)param[2];
  float beta = *(float *)param[3];

  float * C = (float *)data[0];
  float * D = (float *)data[1];
  float * E = (float *)data[2];

  int t_0;
  int t_1;
  int l_0;
  int l_1;
  int k;
  for (t_0 = 0; t_0 < klt_get_tile_length(loop_ctx, 0); t_0 += klt_get_tile_stride(loop_ctx, 0)) {
    for (t_1 = 0; t_1 < klt_get_tile_length(loop_ctx, 1); t_1 += klt_get_tile_stride(loop_ctx, 1)) {
      l_0 = t_0;
      l_1 = t_1;
      E[l_0 * m + l_1] = 0;
      for (k = 0; k < q; k++) {
        E[l_0 * m + l_1] = D[l_0 * q + k] * C[k * m + l_1];
      }
      E[l_0 * m + l_1] *= beta;
    }
  }
}

