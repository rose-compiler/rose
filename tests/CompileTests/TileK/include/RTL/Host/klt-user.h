
#ifndef __TILEK_RTL_KERNEL_H__
#define __TILEK_RTL_KERNEL_H__

struct klt_kernel_desc_t;
struct klt_version_desc_t;
struct klt_deps_desc_t;
struct klt_subkernel_desc_t;

struct klt_kernel_t;

struct klt_loop_context_t;
struct klt_data_context_t;

#if defined(TILEK_BASIC)
typedef void (*tilek_kernel_func_ptr)(void **, void **, struct klt_loop_context_t *, struct klt_data_context_t *);
#elif defined(TILEK_THREADS)
typedef void (*tilek_kernel_func_ptr)(int, void **, void **, struct klt_loop_context_t *, struct klt_data_context_t *);
#endif

struct klt_subkernel_config_t {
#if defined(TILEK_BASIC) || defined(TILEK_THREADS)
  tilek_kernel_func_ptr kernel_ptr;
#elif defined(TILEK_ACCELERATOR)
  char * kernel_name;
#endif
};

struct klt_version_selector_t {};

struct klt_user_config_t {
#if defined(TILEK_THREADS)
  int num_threads;
#elif defined(TILEK_ACCELERATOR)
  int num_gangs[3];
  int num_workers[3];
#endif
};

struct klt_user_config_t * klt_user_build_config(struct klt_kernel_desc_t * desc);

struct klt_version_desc_t * klt_user_select_kernel_version(struct klt_kernel_t * kernel);

void klt_user_schedule(
  struct klt_kernel_t * kernel, struct klt_subkernel_desc_t * subkernel,
  struct klt_loop_context_t * klt_loop_context, struct klt_data_context_t * klt_data_context
);

void klt_user_wait(struct klt_kernel_t * kernel);

int get_length_tile(struct klt_kernel_t * kernel, unsigned long kind);

#endif /* __TILEK_RTL_KERNEL_H__ */

