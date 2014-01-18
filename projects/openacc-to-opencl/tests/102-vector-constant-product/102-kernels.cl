
#include "OpenACC/device/opencl.h"

//////////////////////////////////////////////////////////
// Version with tile #0 dynamic ; tiles #1 and #2 empty //
//////////////////////////////////////////////////////////

__kernel void kernel_102_gang_worker_tile_0(float scalar, __global float * a, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_0;

  // Loop for tile #0
  for (it_loop_0_tile_0  = 0;
       it_loop_0_tile_0  < ctx->loops[0].tiles[e_tile_0].length;
       it_loop_0_tile_0 += ctx->loops[0].tiles[e_tile_0].stride
  ) {
    // Gang "loop"
    long it_loop_0_gang = acc_gang_iteration(ctx, 0, it_loop_0_tile_0);

    // Worker "loop"
    long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_gang);

    a[it_loop_0_worker] *= scalar;
  }
}

//////////////////////////////////////////////////////////
// Version with tile #1 dynamic ; tiles #0 and #2 empty //
//////////////////////////////////////////////////////////

__kernel void kernel_102_gang_worker_tile_1(float scalar, __global float * a, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_1;

  // Gang "loop"
  long it_loop_0_gang = acc_gang_iteration(ctx, 0, 0);

  // Loop for tile #1
  for (it_loop_0_tile_1  = it_loop_0_gang;
       it_loop_0_tile_1  < it_loop_0_gang + ctx->loops[0].tiles[e_tile_1].length;
       it_loop_0_tile_1 +=                  ctx->loops[0].tiles[e_tile_1].stride
  ) {
    // Worker "loop"
    long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_tile_1);

    a[it_loop_0_worker] *= scalar;
  }
}

//////////////////////////////////////////////////////////
// Version with tile #2 dynamic ; tiles #0 and #1 empty //
//////////////////////////////////////////////////////////

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

