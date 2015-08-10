
#ifndef __KLT_RTL_CONTEXT_H__
#define __KLT_RTL_CONTEXT_H__

#ifndef STORAGE_MODIFIER
#define STORAGE_MODIFIER
#endif

struct klt_loop_context_t {
  int num_loops;
  int num_tiles;

  // 3 integers per loops and 2 integers per tiles
  int data[];
};

 int klt_get_loop_lower (STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx);
void klt_set_loop_lower (STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx, int val);

 int klt_get_loop_upper (STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx);
void klt_set_loop_upper (STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx, int val);

 int klt_get_loop_stride(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx);
void klt_set_loop_stride(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx, int val);

 int klt_get_tile_length(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx);
void klt_set_tile_length(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx, int val);

 int klt_get_tile_stride(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx);
void klt_set_tile_stride(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx, int val);

#endif /* __KLT_RTL_CONTEXT_H__ */

