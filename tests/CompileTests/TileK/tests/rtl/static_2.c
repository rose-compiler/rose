
#include "RTL/Host/klt-user.h"

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

struct klt_tile_desc_t tile_desc_k0_v0_0_l1[1] = {
  {0, 1, 0}
};

struct klt_loop_desc_t loops_k0_v0_0[2] = {
  {0, 1, tile_desc_k0_v0_0_l0},
  {0, 1, tile_desc_k0_v0_0_l1}
};

int param_ids_k0_v0_0[4] = {0, 2, 3, 4};
int  data_ids_k0_v0_0[3] = {0, 1, 3};
int loops_ids_k0_v0_0[2] = {0, 1};

extern void kernel_k0_v0_1(void **, void **, struct klt_loop_context_t *, struct klt_data_context_t *);
struct klt_subkernel_config_t config_k0_v0_1[1] = {
  { &kernel_k0_v0_1 }
};

struct klt_tile_desc_t tile_desc_k0_v0_1_l0[1] = {
  {0, 1, 0}
};

struct klt_tile_desc_t tile_desc_k0_v0_1_l1[1] = {
  {0, 1, 0}
};

struct klt_loop_desc_t loops_k0_v0_1[2] = {
  {0, 1, tile_desc_k0_v0_1_l0},
  {0, 1, tile_desc_k0_v0_1_l1}
};

int param_ids_k0_v0_1[4] = {0, 1, 3, 5};
int  data_ids_k0_v0_1[3] = {2, 3, 4};
int loops_ids_k0_v0_1[2] = {2, 3};

struct klt_subkernel_desc_t subkernels_k0_v0[2] = {
  { {2, 2, loops_k0_v0_0}, 4, param_ids_k0_v0_0, 3, data_ids_k0_v0_0, 2, loops_ids_k0_v0_0, config_k0_v0_0},
  { {2, 2, loops_k0_v0_1}, 4, param_ids_k0_v0_1, 3, data_ids_k0_v0_1, 2, loops_ids_k0_v0_1, config_k0_v0_1}
};

int deps_k0_v0_0[0] = {};
int deps_k0_v0_1[1] = {0};

struct klt_deps_desc_t deps_k0_v0[2] = {
  {0, deps_k0_v0_0},
  {1, deps_k0_v0_1}
};

struct klt_version_selector_t version_selector_k0_v0[2] = {{}, {}};

struct klt_version_desc_t versions_k0[1] = {
  {2, subkernels_k0_v0, deps_k0_v0, version_selector_k0_v0}
};

int sizeof_param_k0[6] = {sizeof(int), sizeof(int), sizeof(int), sizeof(int), sizeof(float), sizeof(float)};
int sizeof_data_k0[5] = {sizeof(float), sizeof(float), sizeof(float), sizeof(float), sizeof(float)};
int ndims_data_k0[5] = {2, 2, 2, 2, 2};

struct klt_loop_desc_t loop_desc_k0[4] = {
  {0, 0, 0},
  {1, 0, 0},
  {2, 0, 0},
  {3, 0, 0}
};

struct klt_kernel_desc_t klt_kernel_desc[1] = {
  {
    {6, sizeof_param_k0, 5, sizeof_data_k0, ndims_data_k0},
    {4, 0, loop_desc_k0},
    1, versions_k0
  }
};
