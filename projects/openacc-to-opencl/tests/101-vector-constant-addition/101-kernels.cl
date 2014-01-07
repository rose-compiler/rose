
#include "OpenACC/device/opencl.h"

#if 0
/*!
 *  Generic kernel generated for Offset Addition. Include the 3 levels from OpenACC (gang, worker, vector) and the 4 tiles.
 *  Suffix should be "_gang_worker_vector_tile_0_tile_1_tile_2_tile_3" but by conv this suffix is replaced by ""
 */
__kernel void kernel_101(float offset, __global float * a, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_0;
  long it_loop_0_tile_1;
  long it_loop_0_tile_2;
  long it_loop_0_tile_3;

  long it_loop_0_gang;
  long it_loop_0_worker;
  long it_loop_0_vect;

  // Outer tile loop
  for (it_loop_0_tile_0  = ctx->loops[0].original.lower;
       it_loop_0_tile_0  < ctx->loops[0].original.lower + ctx->loops[0].tiles[e_tile_0].length; // == ctx->loops[0].original.upper
       it_loop_0_tile_0 += ctx->loops[0].tiles[e_tile_0].stride) {

    // Gang "loop"
    it_loop_0_gang = acc_gang_iteration(ctx, 0, it_loop_0_tile_0);

    // Loop for tile between Gang and Worker
    for (it_loop_0_tile_1  = it_loop_0_gang;
         it_loop_0_tile_1  < it_loop_0_gang + ctx->loops[0].tiles[e_tile_1].length;
         it_loop_0_tile_1 += ctx->loops[0].tiles[e_tile_1].stride) {

      // Worker "loop"
      it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_tile_1);

      // Loop for tile between Worker and Vector
      for (it_loop_0_tile_2  = it_loop_0_worker;
           it_loop_0_tile_2  < it_loop_0_worker + ctx->loops[0].tiles[e_tile_2].length;
           it_loop_0_tile_2 += ctx->loops[0].tiles[e_tile_2].stride) {

        // vector loop (real vector loop should be unroll/interchange/fused, need static length to do that)
        for (it_loop_0_vect  = it_loop_0_tile_2;
             it_loop_0_vect  < it_loop_0_tile_2 + ctx->loops[0].tiles[e_vector].length;
             it_loop_0_vect += ctx->loops[0].tiles[e_vector].stride) {

          // Inner tile loop
          for (it_loop_0_tile_3  = it_loop_0_vect;
               it_loop_0_tile_3  < it_loop_0_vect + ctx->loops[0].tiles[e_tile_3].length;
               it_loop_0_tile_3 += ctx->loops[0].tiles[e_tile_3].stride
          ) {
            const long it_loop_0 = it_loop_0_tile_3;
//          if (
//                   it_loop_0 >= ctx->loops[0].original.lower
//               &&  it_loop_0  < ctx->loops[0].original.upper
//               && (it_loop_0  - ctx->loops[0].original.lower) % ctx->loops[0].original.stride == 0
//          ) {
              a[it_loop_0] += offset;
//          }
          }
        }
      }
    }
  }
}
#endif

/*!
 *  Kernel generated for Vector Addition when only considering Gang and Worker (no Vector) and only tile #2 (ie. between Worker and Vector).
 */
__kernel void kernel_101_gang_worker_tile_2(float offset, __global float * a, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_2;

  long it_loop_0_gang = acc_gang_iteration(ctx, 0, 0);

  long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_gang);

  for (it_loop_0_tile_2  = it_loop_0_worker;
       it_loop_0_tile_2  < it_loop_0_worker + ctx->loops[0].tiles[e_tile_2].length;
       it_loop_0_tile_2 += ctx->loops[0].tiles[e_tile_2].stride
  )
    a[it_loop_0_tile_2] += offset;
}

