
#include "RTL/Host/tilek-rtl.h"

#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/data.h"

extern void kernel_k0_v0_0(void **, void **, struct klt_loop_context_t *, struct klt_data_context_t *);

struct klt_subkernel_config_t config_k0_v0_0[1] = {
  { &kernel_k0_v0_0 }
};

struct klt_tile_desc_t tile_desc_k0_v0_0_l0[1] = {
  {0, 1, 0}
};

struct klt_loop_desc_t loops_k0_v0_0[1] = {
  {0, 1, tile_desc_k0_v0_0_l0}
};

int param_ids_k0_v0_0[2] = {0, 1};
int  data_ids_k0_v0_0[1] = {0};
int loops_ids_k0_v0_0[1] = {0};
int  deps_ids_k0_v0_0[0] = {};

struct klt_subkernel_desc_t subkernels_k0_v0[1] = {
  { {1, 1, loops_k0_v0_0}, 2, param_ids_k0_v0_0, 1, data_ids_k0_v0_0, 1, loops_ids_k0_v0_0, 0, deps_ids_k0_v0_0, config_k0_v0_0}
};

struct klt_version_selector_t version_selector_k0_v0[1] = {{}};

struct klt_version_desc_t versions_k0[1] = {
  {1, subkernels_k0_v0, version_selector_k0_v0}
};

int sizeof_param_k0[2] = {sizeof(int), sizeof(float)};
int sizeof_data_k0[1] = {sizeof(float)};
int ndims_data_k0[1] = {1};

struct klt_loop_desc_t loop_desc_k0[1] = {
  {0, 0, 0}
};

struct klt_kernel_desc_t klt_kernel_desc[1] = {
  {
    {2, sizeof_param_k0, 1, sizeof_data_k0, ndims_data_k0},
    {1, 0, loop_desc_k0},
    1, versions_k0
  }
};

