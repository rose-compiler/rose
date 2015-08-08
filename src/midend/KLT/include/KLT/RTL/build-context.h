
#ifndef __KLT_RTL_SOLVE_LOOP_CONTEXT_H__
#define __KLT_RTL_SOLVE_LOOP_CONTEXT_H__

struct klt_kernel_t;
struct klt_loop_t;
struct klt_loop_container_t;
struct klt_loop_context_t;
struct klt_data_context_t;

struct klt_loop_context_t * klt_build_loop_context(struct klt_loop_container_t * loop_container, struct klt_loop_t * loops, struct klt_kernel_t * kernel);

struct klt_data_context_t * klt_build_data_context();

#endif /* __KLT_RTL_SOLVE_LOOP_CONTEXT_H__ */

