
#include "RTL/Host/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

void launch(struct kernel_t * kernel, struct klt_loop_context_t * klt_loop_context) {
  void ** local_private = NULL; // TODO
  (*kernel->desc->kernel_ptr)(kernel->param, kernel->scalar, kernel->data, local_private, klt_loop_context);
}

int get_length_tile(struct kernel_t * kernel, unsigned long kind) {
  assert(0);
}

