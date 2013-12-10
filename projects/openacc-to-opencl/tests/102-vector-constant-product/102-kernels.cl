
#include "OpenACC/device/opencl.h"

__kernel void kernel_102_gang_worker_tile_2(float scalar, __global float * a, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_2;

  long it_loop_0_gang = acc_gang_iteration(ctx, 0, 0);

  long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_gang);

  for (it_loop_0_tile_2  = it_loop_0_worker;
       it_loop_0_tile_2  < it_loop_0_worker + ctx->loops[0].tiles[e_tile_2].length;
       it_loop_0_tile_2 += ctx->loops[0].tiles[e_tile_2].stride
  )
    a[it_loop_0_tile_2] *= scalar;
}

