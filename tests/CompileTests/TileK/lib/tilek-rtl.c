
#include "RTL/Host/tilek-rtl.h"

#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/data.h"
#include "KLT/RTL/context.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

struct klt_version_desc_t * klt_user_select_kernel_version(struct klt_kernel_t * kernel) {
  assert(kernel->desc->num_versions == 1);
  return &(kernel->desc->versions[0]);
}

