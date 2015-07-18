
#include "KLT/RTL/context.h"

int klt_get_loop_lower(struct klt_loop_context_t * ctx, int idx) {
  return ctx->data[3 * idx];
}

void klt_set_loop_lower(struct klt_loop_context_t * ctx, int idx, int val) {
  ctx->data[3 * idx] = val;
}

int klt_get_loop_upper(struct klt_loop_context_t * ctx, int idx) {
  return ctx->data[3 * idx + 1];
}

void klt_set_loop_upper(struct klt_loop_context_t * ctx, int idx, int val) {
  ctx->data[3 * idx + 1] = val;
}

int klt_get_loop_stride(struct klt_loop_context_t * ctx, int idx) {
  return ctx->data[3 * idx + 2];
}

void klt_set_loop_stride(struct klt_loop_context_t * ctx, int idx, int val) {
  ctx->data[3 * idx + 2] = val;
}

int klt_get_tile_length(struct klt_loop_context_t * ctx, int idx) {
  return ctx->data[3 * ctx->num_loops + 2 * idx];
}

void klt_set_tile_length(struct klt_loop_context_t * ctx, int idx, int val) {
  ctx->data[3 * ctx->num_loops + 2 * idx] = val;
}

int klt_get_tile_stride(struct klt_loop_context_t * ctx, int idx) {
  return ctx->data[3 * ctx->num_loops + 2 * idx + 1];
}

void klt_set_tile_stride(struct klt_loop_context_t * ctx, int idx, int val) {
  ctx->data[3 * ctx->num_loops + 2 * idx + 1] = val;
}

