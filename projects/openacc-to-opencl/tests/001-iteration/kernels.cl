
#include "OpenACC/device/opencl.h"

/*!
 *  Kernel generated for Vector Addition when only considering Gang and Worker (no Vector) and only tile #2 (ie. between Worker and Vector).
 */
__kernel void iteration_kernel_gang_8_worker_64(__global int * a, __global int * b, __constant struct acc_context_t_ * ctx) {
  // Gang "loop"
  long it_loop_0_gang = acc_gang_iteration(ctx, 0, 0);

  // Worker "loop"
  long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_gang);

  a[it_loop_0_worker] = acc_gang_id(ctx);
  b[it_loop_0_worker] = acc_worker_id(ctx);
}

