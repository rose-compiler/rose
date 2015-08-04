
#include "KLT/RTL/context.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/build-context.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

struct klt_kernel_t;

extern int get_length_tile(struct klt_kernel_t * kernel, unsigned long kind);

void klt_solve_loop_context(int num_loops, struct klt_loop_desc_t * loop_desc, struct klt_loop_context_t * loop_ctx, struct klt_kernel_t * kernel) {
  int loop_it, tile_it;

  for (loop_it = 0; loop_it < num_loops; loop_it++) {
    if (loop_desc[loop_it].num_tiles == 0) continue;

    const int loop_idx = loop_desc[loop_it].idx;

    int length = klt_get_loop_upper(loop_ctx, loop_idx) - klt_get_loop_lower(loop_ctx, loop_idx) + 1; // Length of the loop
    int last_tile = loop_desc[loop_it].tile_desc[loop_desc[loop_it].num_tiles-1].idx; // Stride of the loop
    klt_set_tile_stride(loop_ctx, last_tile, klt_get_loop_stride(loop_ctx, loop_idx));

    int dyn_tile = loop_desc[loop_it].num_tiles;

    for (tile_it = 0; tile_it < loop_desc[loop_it].num_tiles; tile_it++) {
      const int tile_idx = loop_desc[loop_it].tile_desc[tile_it].idx;
      int param = 0;
      switch (loop_desc[loop_it].tile_desc[tile_it].kind) {
        case e_tile_static:
          param = loop_desc[loop_it].tile_desc[tile_it].param;
          break;
        case e_tile_dynamic:
          break;
        default:
          param = get_length_tile(kernel, loop_desc[loop_it].tile_desc[tile_it].kind);
          assert(param > 0);
      }

      klt_set_tile_length(loop_ctx, tile_idx, length);

      if (param == 0) {
        dyn_tile = tile_it;
        break;
      }

      if (klt_get_tile_stride(loop_ctx, tile_idx) == 0)
         klt_set_tile_stride(loop_ctx, tile_idx, length / param);
      else assert(klt_get_tile_stride(loop_ctx, tile_idx) == length / param);

      assert(klt_get_tile_stride(loop_ctx, tile_idx)  > 0);

      //klt_set_tile_remain(loop_ctx, tile_idx, length % klt_get_tile_stride(loop_ctx, tile_idx));

      length = klt_get_tile_stride(loop_ctx, tile_idx);
    }

    if (dyn_tile != loop_desc[loop_it].num_tiles) {
      int stride = klt_get_loop_stride(loop_ctx, loop_idx);
      for (tile_it = loop_desc[loop_it].num_tiles - 1; tile_it > dyn_tile; tile_it--) {
        const int tile_idx = loop_desc[loop_it].tile_desc[tile_it].idx;
        int param = 0;
        switch (loop_desc[loop_it].tile_desc[tile_it].kind) {
          case e_tile_static:
            param = loop_desc[loop_it].tile_desc[tile_it].param;
            break;
          case e_tile_dynamic:
            assert(0);
            break;
          default:
            param = get_length_tile(kernel, loop_desc[loop_it].tile_desc[tile_it].kind);
            assert(param > 0);
        }

        assert(loop_desc[loop_it].tile_desc[tile_it].kind != e_tile_dynamic);

        if (klt_get_tile_stride(loop_ctx, tile_idx) == 0)
          klt_set_tile_stride(loop_ctx, tile_idx, stride);
        else assert(klt_get_tile_stride(loop_ctx, tile_idx) == stride);

        if (klt_get_tile_length(loop_ctx, tile_idx) == 0) {
          klt_set_tile_length(loop_ctx, tile_idx, stride * param);
          //klt_set_tile_remain(loop_ctx, tile_idx, 0);
        }
        else assert(klt_get_tile_length(loop_ctx, tile_idx) == stride * param);

        stride = klt_get_tile_length(loop_ctx, tile_idx);
      }

      const int tile_idx = loop_desc[loop_it].tile_desc[dyn_tile].idx;
      if (dyn_tile == loop_desc[loop_it].num_tiles - 1) {
        klt_set_tile_stride(loop_ctx, tile_idx, klt_get_loop_stride(loop_ctx, loop_idx));
      }
      else if (dyn_tile == 0) {
        assert(loop_desc[loop_it].num_tiles > 1);
        int l1 = klt_get_loop_upper(loop_ctx, loop_idx) - klt_get_loop_lower(loop_ctx, loop_idx) + 1;
        int l2 = klt_get_tile_length(loop_ctx, loop_desc[loop_it].tile_desc[1].idx);
        klt_set_tile_stride(loop_ctx, tile_idx, l1 / l2);
      }
      else klt_set_tile_stride(loop_ctx, tile_idx, stride);
    }
  }
}

struct klt_loop_context_t * klt_build_loop_context(struct klt_loop_container_t * loop_container, struct klt_loop_t * loops_, struct klt_kernel_t * kernel) {
  struct klt_loop_t * loops = malloc(loop_container->num_loops * sizeof(struct klt_loop_t));
  int i;
  for (i = 0; i < loop_container->num_loops; i++)
    loops[i] = loops_[loop_container->loop_desc[i].idx];

  int size = sizeof(struct klt_loop_context_t) + loop_container->num_loops * sizeof(struct klt_loop_t) + loop_container->num_tiles * sizeof(struct klt_tile_t);
  struct klt_loop_context_t * loop_ctx = malloc(size);

  memset(loop_ctx, 0, size);

  loop_ctx->num_loops = loop_container->num_loops;
  loop_ctx->num_tiles = loop_container->num_tiles;
  memcpy(loop_ctx->data, loops, loop_container->num_loops * sizeof(struct klt_loop_t));

  klt_solve_loop_context(loop_container->num_loops, loop_container->loop_desc, loop_ctx, kernel);

  return loop_ctx;
}

struct klt_data_context_t * klt_build_data_context() {
  return malloc(sizeof(struct klt_data_context_t));
}

