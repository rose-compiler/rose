
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TileK/kernel.h"
#include "TileK/loop.h"
#include "TileK/tile.h"

struct kernel_t * build_kernel(int idx) {
  struct kernel_desc_t * desc = &(kernel_desc[idx]);

  int size = sizeof(struct kernel_t)
           + desc->num_data  * sizeof(void *)
           + desc->num_param * sizeof(int)
           + desc->num_loops * sizeof(struct loop_t)
           + desc->num_tiles * sizeof(struct tile_t);

  void * alloc = malloc(size);

  memset(alloc, 0, size);

  struct kernel_t * res = (struct kernel_t *)alloc;
      res->desc  = desc;
      res->data  =         (void **)(alloc += sizeof(struct kernel_t));
      res->param =           (int *)(alloc += desc->num_data  * sizeof(void *));
      res->loops = (struct loop_t *)(alloc += desc->num_param * sizeof(int));
      res->tiles = (struct tile_t *)(alloc += desc->num_loops * sizeof(struct loop_t));

  return res;
}

void execute_kernel(struct kernel_t * kernel) {
  const struct kernel_desc_t * desc = kernel->desc;

  int loop_it, tile_it;

  for (loop_it = 0; loop_it < desc->num_loops; loop_it++) {
    const int loop_idx = desc->loop_desc[loop_it].idx;

    int length = kernel->loops[loop_idx].upper - kernel->loops[loop_idx].lower;
//  printf("[TILEK] loops[%d] = %d to %d (%d) : %d tiles\n", loop_it, kernel->loops[loop_idx].lower, kernel->loops[loop_idx].upper, length, desc->loop_desc[loop_it].num_tiles);

    kernel->tiles[desc->loop_desc[loop_it].num_tiles-1].stride = kernel->loops[loop_idx].stride;

    int dyn_tile = desc->loop_desc[loop_it].num_tiles;

    for (tile_it = 0; tile_it < desc->loop_desc[loop_it].num_tiles; tile_it++) {
      const int tile_idx = desc->loop_desc[loop_it].tile_desc[tile_it].idx;
      const int param = desc->loop_desc[loop_it].tile_desc[tile_it].param;

//    printf("[TILEK]   loops[%d].tiles[%d] : #%d\n", loop_idx, tile_it, tile_idx);

      kernel->tiles[tile_idx].length = length;

      if (desc->loop_desc[loop_it].tile_desc[tile_it].kind == e_tile_dynamic) {
        dyn_tile = tile_it;
        break;
      }

      if (kernel->tiles[tile_idx].stride == 0)
         kernel->tiles[tile_idx].stride = length / param;
      else assert(kernel->tiles[tile_idx].stride == length / param);

      assert(kernel->tiles[tile_idx].stride > 0);

//    printf("[TILEK]   > #%d : %d by %d\n", tile_idx, loop_idx, tile_it);

//    kernel->tiles[tile_idx].remain = length % kernel->tiles[tile_idx].stride;

      length = kernel->tiles[tile_idx].stride;
    }

    if (dyn_tile != desc->loop_desc[loop_it].num_tiles) {
//    printf("[TILEK] Dynamic: loops[%d].tiles[%d] : #%d\n", loop_idx, dyn_tile, desc->loop_desc[loop_it].tile_desc[dyn_tile].idx);

      int stride = kernel->loops[loop_idx].stride;
      for (tile_it = desc->loop_desc[loop_it].num_tiles - 1; tile_it > dyn_tile; tile_it--) {
        const int tile_idx = desc->loop_desc[loop_it].tile_desc[tile_it].idx;
        const int param = desc->loop_desc[loop_it].tile_desc[tile_it].param;

//      printf("[TILEK]   loops[%d].tiles[%d] : #%d\n", loop_idx, tile_it, tile_idx);

        if (desc->loop_desc[loop_it].tile_desc[tile_it].kind == e_tile_dynamic) {
          assert(dyn_tile == loop_it);
          break;
        }

        if (kernel->tiles[tile_idx].stride == 0)
          kernel->tiles[tile_idx].stride = stride;
        else assert(kernel->tiles[tile_idx].stride == stride);

        if (kernel->tiles[tile_idx].length == 0) {
          kernel->tiles[tile_idx].length = stride * param;
//        kernel->tiles[tile_idx].remain = 0;
        }
        else assert(kernel->tiles[tile_idx].length = stride * param);

//    printf("[TILEK]   > #%d : %d by %d\n", tile_idx, loop_idx, tile_it);

        stride = kernel->tiles[tile_idx].length;
      }

      const int tile_idx = desc->loop_desc[loop_it].tile_desc[dyn_tile].idx;
      if (dyn_tile == desc->loop_desc[loop_it].num_tiles - 1) {
        kernel->tiles[tile_idx].stride = kernel->loops[loop_idx].stride;
      }
      else if (dyn_tile == 0) {
        assert(desc->loop_desc[loop_it].num_tiles > 1);
        kernel->tiles[tile_idx].stride = (kernel->loops[loop_idx].upper - kernel->loops[loop_idx].lower) / kernel->tiles[desc->loop_desc[loop_it].tile_desc[1].idx].length;
      }
      else {
        assert(desc->loop_desc[loop_it].num_tiles > 2);
        assert(dyn_tile < desc->loop_desc[loop_it].num_tiles - 1);
        assert(dyn_tile > 0);
        kernel->tiles[tile_idx].stride = kernel->tiles[desc->loop_desc[loop_it].tile_desc[dyn_tile - 1].idx].stride / kernel->tiles[desc->loop_desc[loop_it].tile_desc[dyn_tile + 1].idx].length;
      }
//    printf("[TILEK] > (DYN) loops[%d].tiles[%d] = %d by %d : #%d\n", loop_idx, dyn_tile, kernel->tiles[tile_idx].length, kernel->tiles[tile_idx].stride, tile_idx);
    }
  }

  struct context_t context = { kernel->loops, kernel->tiles };
  (*desc->kernel_ptr)(kernel->data, kernel->param, &context);
}

