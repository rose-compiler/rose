
#ifndef __KLT_RTL_KERNEL_H__
#define __KLT_RTL_KERNEL_H__

#include "KLT/RTL/loop.h"
#include "KLT/RTL/data.h"

struct klt_subkernel_config_t;
struct klt_version_selector_t;
struct klt_user_config_t;

struct klt_subkernel_desc_t {
  struct klt_loop_container_t loop;
  int num_params;
  int * param_ids;
  int num_data;
  int * data_ids;
  int num_loops;
  int * loop_ids;
  int num_deps;
  int * deps_ids;
  struct klt_subkernel_config_t * config;
};

struct klt_version_desc_t {
  int num_subkernels;
  struct klt_subkernel_desc_t * subkernels;
  struct klt_version_selector_t * version_selector;
};

struct klt_kernel_desc_t {
  struct klt_data_container_t data;
  struct klt_loop_container_t loop;
  int num_versions;
  struct klt_version_desc_t * versions;
};

struct klt_kernel_t {
  struct klt_kernel_desc_t * desc;

  void ** param;
  struct klt_data_t * data;

  struct klt_loop_t * loops;

  struct klt_user_config_t * config;
};

extern struct klt_kernel_desc_t klt_kernel_desc[];

struct klt_kernel_t * klt_build_kernel(int idx);

void klt_execute_kernel(struct klt_kernel_t * kernel);

#endif /* __KLT_RTL_KERNEL_H__ */

