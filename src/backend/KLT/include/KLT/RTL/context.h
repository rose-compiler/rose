
#ifndef __KLT_RTL_CONTEXT_H__
#define __KLT_RTL_CONTEXT_H__

struct klt_loop_context_t {
  int num_loops;
  int num_tiles;

  // 3 integers per loops and 2 integers per tiles
  int data[];
};

 int klt_get_loop_lower (struct klt_loop_context_t * ctx, int idx);
void klt_set_loop_lower (struct klt_loop_context_t * ctx, int idx, int val);

 int klt_get_loop_upper (struct klt_loop_context_t * ctx, int idx);
void klt_set_loop_upper (struct klt_loop_context_t * ctx, int idx, int val);

 int klt_get_loop_stride(struct klt_loop_context_t * ctx, int idx);
void klt_set_loop_stride(struct klt_loop_context_t * ctx, int idx, int val);

 int klt_get_tile_length(struct klt_loop_context_t * ctx, int idx);
void klt_set_tile_length(struct klt_loop_context_t * ctx, int idx, int val);

 int klt_get_tile_stride(struct klt_loop_context_t * ctx, int idx);
void klt_set_tile_stride(struct klt_loop_context_t * ctx, int idx, int val);

/*struct klt_data_context_t {
  // 
};*/

#endif /* __KLT_RTL_CONTEXT_H__ */

