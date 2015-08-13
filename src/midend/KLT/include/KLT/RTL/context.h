
#ifndef __KLT_RTL_CONTEXT_H__
#define __KLT_RTL_CONTEXT_H__

#if !defined(COMPILE_FOR_KERNEL)
#  define COMPILE_FOR_KERNEL 0
#endif

#if COMPILE_FOR_KERNEL == 0
#  if defined(STORAGE_MODIFIER)
#    error
#  endif
#  define STORAGE_MODIFIER
#else
#  if !defined(STORAGE_MODIFIER)
#    error
#  endif
#endif

struct klt_loop_context_t {
  int num_loops;
  int num_tiles;

  // 3 integers per loops and 2 integers per tiles
  int data[];
};

int klt_get_loop_lower (STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx);
int klt_get_loop_upper (STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx);
int klt_get_loop_stride(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx);
int klt_get_tile_length(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx);
int klt_get_tile_stride(STORAGE_MODIFIER struct klt_loop_context_t * ctx, int idx);

#if COMPILE_FOR_KERNEL == 0
void klt_set_loop_lower (struct klt_loop_context_t * ctx, int idx, int val);
void klt_set_loop_upper (struct klt_loop_context_t * ctx, int idx, int val);
void klt_set_loop_stride(struct klt_loop_context_t * ctx, int idx, int val);
void klt_set_tile_length(struct klt_loop_context_t * ctx, int idx, int val);
void klt_set_tile_stride(struct klt_loop_context_t * ctx, int idx, int val);
#endif

struct klt_data_context_t {
  int num_datas; // OpenCL don't like it if it is empty
};

#endif /* __KLT_RTL_CONTEXT_H__ */

