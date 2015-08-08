
#ifndef __KLT_RTL_H__
#define __KLT_RTL_H__

// Kernel
struct klt_subkernel_desc_t;
struct klt_deps_desc_t;
struct klt_version_desc_t ;
struct klt_kernel_desc_t;
struct klt_kernel_t;

// Loop
struct klt_loop_container_t;
struct klt_loop_desc_t;
struct klt_loop_t;

// Tile
struct klt_tile_desc_t;
struct klt_tile_t;

// Data
struct klt_data_container_t;
struct klt_data_section_t;
struct klt_data_t;

// Interface
extern struct klt_kernel_t * klt_build_kernel(int idx);
extern void klt_execute_kernel(struct klt_kernel_t * kernel);

// User defined type
struct klt_subkernel_config_t;
struct klt_version_selector_t;
struct klt_user_config_t;

// User defined runtime function
extern struct klt_user_config_t * klt_user_build_config(struct klt_kernel_desc_t * desc);
extern void klt_user_wait(struct klt_kernel_t * kernel);
extern void klt_user_schedule(struct klt_kernel_t * kernel, struct klt_version_desc_t * version, struct klt_loop_context_t * klt_loop_context, struct klt_data_context_t * klt_data_context);
extern struct klt_version_desc_t * klt_user_select_kernel_version(struct klt_kernel_t * kernel);
extern int klt_user_get_tile_length(struct klt_kernel_t * kernel, unsigned long kind, unsigned long param);

// Compiler generated
extern struct klt_kernel_desc_t klt_kernel_desc[];

#endif /* __KLT_RTL_H__ */

