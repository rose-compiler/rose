
#include "RTL/Host/tilek-rtl.h"

#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/data.h"
#include "KLT/RTL/context.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

struct klt_user_config_t * klt_user_build_config(struct klt_kernel_desc_t * desc)  {
  struct klt_user_config_t * config = malloc(sizeof(struct klt_user_config_t));
    config->num_gangs[0] = 1;
    config->num_gangs[1] = 1;
    config->num_gangs[2] = 1;
    config->num_workers[0] = 1;
    config->num_workers[1] = 1;
    config->num_workers[2] = 1;
  return config;
}

int klt_user_get_tile_length(struct klt_kernel_t * kernel, unsigned long kind, unsigned long param) {
  assert(kind > 1 && kind < 8);
  switch (kind) {
    case 2:
      assert(kernel->config->num_gangs[param] > 0);
      return kernel->config->num_gangs[param];
    case 3:
      assert(kernel->config->num_workers[param] > 0);
      return kernel->config->num_workers[param];
    default: assert(0);
  }
}

