
#include "KLT/RTL/context.h"

DEVICE_FUNCTION_MODIFIER int klt_get_loop_lower(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx) {
  return ctx->data[3 * idx];
}

DEVICE_FUNCTION_MODIFIER int klt_get_loop_upper(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx) {
  return ctx->data[3 * idx + 1];
}

DEVICE_FUNCTION_MODIFIER int klt_get_loop_stride(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx) {
  return ctx->data[3 * idx + 2];
}

DEVICE_FUNCTION_MODIFIER int klt_get_tile_length(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx) {
  return ctx->data[3 * ctx->num_loops + 2 * idx];
}

DEVICE_FUNCTION_MODIFIER int klt_get_tile_stride(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx) {
  return ctx->data[3 * ctx->num_loops + 2 * idx + 1];
}

#if COMPILE_FOR_KERNEL == 0

void klt_set_loop_lower(struct klt_loop_context_t * ctx, int idx, int val) {
  ctx->data[3 * idx] = val;
}

void klt_set_loop_upper(struct klt_loop_context_t * ctx, int idx, int val) {
  ctx->data[3 * idx + 1] = val;
}

void klt_set_loop_stride(struct klt_loop_context_t * ctx, int idx, int val) {
  ctx->data[3 * idx + 2] = val;
}

void klt_set_tile_length(struct klt_loop_context_t * ctx, int idx, int val) {
  ctx->data[3 * ctx->num_loops + 2 * idx] = val;
}

void klt_set_tile_stride(struct klt_loop_context_t * ctx, int idx, int val) {
  ctx->data[3 * ctx->num_loops + 2 * idx + 1] = val;
}

#endif

