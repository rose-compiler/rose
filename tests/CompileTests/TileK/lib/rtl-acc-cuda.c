
#include "RTL/Host/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"

// TODO CUDA low-level API header

#include <stdlib.h>
#include <string.h>

#include <assert.h>

void launch(struct kernel_t * kernel, struct klt_loop_context_t * klt_loop_context) {
  assert(0); // TODO implement using CUDA low-level API
}

