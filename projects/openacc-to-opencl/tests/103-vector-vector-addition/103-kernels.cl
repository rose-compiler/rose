
#include "OpenACC/device/opencl.h"
#if 0
/*!
 *  Generic kernel generated for Vector Addition. Include the 3 levels from OpenACC (gang, worker, vector) and the 4 tiles.
 *  Suffix should be "_gang_worker_vector_tile_0_tile_1_tile_2_tile_3" but by conv this suffix is replaced by ""
 */
__kernel void kernel_103(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_0;
  long it_loop_0_tile_1;
  long it_loop_0_tile_2;
  long it_loop_0_tile_3;

  long it_loop_0_vect;

  // Outer tile loop
  for (it_loop_0_tile_0  = ctx->loops[0].original.lower;
       it_loop_0_tile_0  < ctx->loops[0].original.upper;
       it_loop_0_tile_0 += ctx->loops[0].tiles[e_tile_0].stride) {

    // Gang "loop"
    const long it_loop_0_gang = acc_gang_iteration(ctx, 0, it_loop_0_tile_0);

    // Loop for tile between Gang and Worker
    for (it_loop_0_tile_1  = it_loop_0_gang;
         it_loop_0_tile_1  < it_loop_0_gang + ctx->loops[0].tiles[e_tile_1].length;
         it_loop_0_tile_1 += ctx->loops[0].tiles[e_tile_1].stride) {

      // Worker "loop"
      const long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_tile_1);

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
              res[it_loop_0] = a[it_loop_0] + b[it_loop_0];
//          }
          }
        }
      }
    }
  }
}
#endif

//////////////////////////////////////////////////////////
// Version with tile #0 dynamic ; tiles #1 and #2 empty //
//////////////////////////////////////////////////////////

__kernel void kernel_103_gang_worker_tile_0(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
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

    res[it_loop_0_worker] = a[it_loop_0_worker] + b[it_loop_0_worker];
  }
}

__kernel void kernel_103_gang_worker_tile_0_vector_2(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
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

    float2 a_2 = vload2(it_loop_0_worker, a);
    float2 b_2 = vload2(it_loop_0_worker, b);
    float2 res_2 = a_2 + b_2;
    vstore2(res_2, it_loop_0_worker, res);
  }
}

__kernel void kernel_103_gang_worker_tile_0_vector_4(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
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

    float4 a_4 = vload4(it_loop_0_worker, a);
    float4 b_4 = vload4(it_loop_0_worker, b);
    float4 res_4 = a_4 + b_4;
    vstore4(res_4, it_loop_0_worker, res);
  }
}

__kernel void kernel_103_gang_worker_tile_0_vector_8(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
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

    float8 a_8 = vload8(it_loop_0_worker, a);
    float8 b_8 = vload8(it_loop_0_worker, b);
    float8 res_8 = a_8 + b_8;
    vstore8(res_8, it_loop_0_worker, res);
  }
}

__kernel void kernel_103_gang_worker_tile_0_vector_16(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
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

    float16 a_16 = vload16(it_loop_0_worker, a);
    float16 b_16 = vload16(it_loop_0_worker, b);
    float16 res_16 = a_16 + b_16;
    vstore16(res_16, it_loop_0_worker, res);
  }
}

//////////////////////////////////////////////////////////
// Version with tile #1 dynamic ; tiles #0 and #2 empty //
//////////////////////////////////////////////////////////1

__kernel void kernel_103_gang_worker_tile_1(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
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

    res[it_loop_0_worker] = a[it_loop_0_worker] + b[it_loop_0_worker];
  }
}

__kernel void kernel_103_gang_worker_tile_1_vector_2(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
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

    float2 a_2 = vload2(it_loop_0_worker, a);
    float2 b_2 = vload2(it_loop_0_worker, b);
    float2 res_2 = a_2 + b_2;
    vstore2(res_2, it_loop_0_worker, res);
  }
}

__kernel void kernel_103_gang_worker_tile_1_vector_4(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
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

    float4 a_4 = vload4(it_loop_0_worker, a);
    float4 b_4 = vload4(it_loop_0_worker, b);
    float4 res_4 = a_4 + b_4;
    vstore4(res_4, it_loop_0_worker, res);
  }
}

__kernel void kernel_103_gang_worker_tile_1_vector_8(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
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

    float8 a_8 = vload8(it_loop_0_worker, a);
    float8 b_8 = vload8(it_loop_0_worker, b);
    float8 res_8 = a_8 + b_8;
    vstore8(res_8, it_loop_0_worker, res);
  }
}

__kernel void kernel_103_gang_worker_tile_1_vector_16(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
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

    float16 a_16 = vload16(it_loop_0_worker, a);
    float16 b_16 = vload16(it_loop_0_worker, b);
    float16 res_16 = a_16 + b_16;
    vstore16(res_16, it_loop_0_worker, res);
  }
}

//////////////////////////////////////////////////////////
// Version with tile #2 dynamic ; tiles #0 and #1 empty //
//////////////////////////////////////////////////////////

__kernel void kernel_103_gang_worker_tile_2(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_2;

  // Gang "loop"
  long it_loop_0_gang = acc_gang_iteration(ctx, 0, 0);

  // Worker "loop"
  long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_gang);

  // Loop for tile between Worker and Vector
  for (it_loop_0_tile_2  = it_loop_0_worker;
       it_loop_0_tile_2  < it_loop_0_worker + ctx->loops[0].tiles[e_tile_2].length;
       it_loop_0_tile_2 +=                    ctx->loops[0].tiles[e_tile_2].stride
  )
    res[it_loop_0_tile_2] = a[it_loop_0_tile_2] + b[it_loop_0_tile_2];
}

__kernel void kernel_103_gang_worker_tile_2_vector_2(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_2;

  // Gang "loop"
  long it_loop_0_gang = acc_gang_iteration(ctx, 0, 0);

  // Worker "loop"
  long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_gang);

  // Loop for tile between Worker and Vector
  for (it_loop_0_tile_2  = it_loop_0_worker;
       it_loop_0_tile_2  < it_loop_0_worker + ctx->loops[0].tiles[e_tile_2].length;
       it_loop_0_tile_2 +=                    ctx->loops[0].tiles[e_tile_2].stride
  ) {
    float2 a_2 = vload2(it_loop_0_tile_2, a);
    float2 b_2 = vload2(it_loop_0_tile_2, b);
    float2 res_2 = a_2 + b_2;
    vstore2(res_2, it_loop_0_tile_2, res);
  }
}

__kernel void kernel_103_gang_worker_tile_2_vector_4(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_2;

  // Gang "loop"
  long it_loop_0_gang = acc_gang_iteration(ctx, 0, 0);

  // Worker "loop"
  long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_gang);

  // Loop for tile between Worker and Vector
  for (it_loop_0_tile_2  = it_loop_0_worker;
       it_loop_0_tile_2  < it_loop_0_worker + ctx->loops[0].tiles[e_tile_2].length;
       it_loop_0_tile_2 +=                    ctx->loops[0].tiles[e_tile_2].stride
  ) {
    float4 a_4 = vload4(it_loop_0_tile_2, a);
    float4 b_4 = vload4(it_loop_0_tile_2, b);
    float4 res_4 = a_4 + b_4;
    vstore4(res_4, it_loop_0_tile_2, res);
  }
}

__kernel void kernel_103_gang_worker_tile_2_vector_8(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_2;

  // Gang "loop"
  long it_loop_0_gang = acc_gang_iteration(ctx, 0, 0);

  // Worker "loop"
  long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_gang);

  // Loop for tile between Worker and Vector
  for (it_loop_0_tile_2  = it_loop_0_worker;
       it_loop_0_tile_2  < it_loop_0_worker + ctx->loops[0].tiles[e_tile_2].length;
       it_loop_0_tile_2 +=                    ctx->loops[0].tiles[e_tile_2].stride
  ) {
    float8 a_8 = vload8(it_loop_0_tile_2, a);
    float8 b_8 = vload8(it_loop_0_tile_2, b);
    float8 res_8 = a_8 + b_8;
    vstore8(res_8, it_loop_0_tile_2, res);
  }
}

__kernel void kernel_103_gang_worker_tile_2_vector_16(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_2;

  // Gang "loop"
  long it_loop_0_gang = acc_gang_iteration(ctx, 0, 0);

  // Worker "loop"
  long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_gang);

  // Loop for tile between Worker and Vector
  for (it_loop_0_tile_2  = it_loop_0_worker;
       it_loop_0_tile_2  < it_loop_0_worker + ctx->loops[0].tiles[e_tile_2].length;
       it_loop_0_tile_2 +=                    ctx->loops[0].tiles[e_tile_2].stride
  ) {
    float16 a_16 = vload16(it_loop_0_tile_2, a);
    float16 b_16 = vload16(it_loop_0_tile_2, b);
    float16 res_16 = a_16 + b_16;
    vstore16(res_16, it_loop_0_tile_2, res);
  }
}

//////////////////////////////////////////////
// Version with tiles #0, #1 and #2 dynamic // Used static configurations (like 2,n,64 or 8,128,n)
//////////////////////////////////////////////

__kernel void kernel_103_gang_worker_tile_0_1_2(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_0;
  long it_loop_0_tile_1;
  long it_loop_0_tile_2;

  // Outer tile loop
  for (it_loop_0_tile_0  = 0;
       it_loop_0_tile_0  < ctx->loops[0].tiles[e_tile_0].length;
       it_loop_0_tile_0 += ctx->loops[0].tiles[e_tile_0].stride) {

    // Gang "loop"
    const long it_loop_0_gang = acc_gang_iteration(ctx, 0, it_loop_0_tile_0);

    // Loop for tile between Gang and Worker
    for (it_loop_0_tile_1  = it_loop_0_gang;
         it_loop_0_tile_1  < it_loop_0_gang + ctx->loops[0].tiles[e_tile_1].length;
         it_loop_0_tile_1 +=                  ctx->loops[0].tiles[e_tile_1].stride) {

      // Worker "loop"
      const long it_loop_0_worker = acc_worker_iteration(ctx, 0, it_loop_0_tile_1);

      // Loop for tile between Worker and Vector
      for (it_loop_0_tile_2  = it_loop_0_worker;
           it_loop_0_tile_2  < it_loop_0_worker + ctx->loops[0].tiles[e_tile_2].length;
           it_loop_0_tile_2 +=                    ctx->loops[0].tiles[e_tile_2].stride)

              res[it_loop_0_tile_2] = a[it_loop_0_tile_2] + b[it_loop_0_tile_2];
    }
  }
}

