
#ifndef __KLT_RTL_SOLVE_LOOP_CONTEXT_H__
#define __KLT_RTL_SOLVE_LOOP_CONTEXT_H__

struct klt_loop_context_t * klt_build_loop_context(int num_loops, int num_tiles, struct klt_loop_desc_t * loop_desc, struct klt_loop_t * loops, struct kernel_t * kernel);

#endif /* __KLT_RTL_SOLVE_LOOP_CONTEXT_H__ */

