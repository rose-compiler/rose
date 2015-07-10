
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "TileK/kernel.h"
#include "TileK/loop.h"
#include "TileK/tile.h"

struct kernel_t * build_kernel(int idx) {
  struct kernel_desc_t * desc = &(kernel_desc[idx]);

  int size = sizeof(struct kernel_t)
           + desc->num_data   * sizeof(void *)
           + desc->num_param  * sizeof(int)
           + desc->num_scalar * sizeof(void *)
           + desc->num_loops  * sizeof(struct loop_t)
           + desc->num_tiles  * sizeof(struct tile_t);

  void * alloc = malloc(size);

  memset(alloc, 0, size);

  struct kernel_t * res = (struct kernel_t *)alloc;
      res->desc   = desc;
      res->data   =         (void **)(alloc += sizeof(struct kernel_t));
      res->param  =           (int *)(alloc += desc->num_data   * sizeof(void *));
      res->scalar =           (int *)(alloc += desc->num_param  * sizeof(int));
      res->loops  = (struct loop_t *)(alloc += desc->num_scalar * sizeof(void *));
      res->tiles  = (struct tile_t *)(alloc += desc->num_loops  * sizeof(struct loop_t));

  return res;
}

void execute_kernel(struct kernel_t * kernel) {
  const struct kernel_desc_t * desc = kernel->desc;

  int loop_it, tile_it;

  for (loop_it = 0; loop_it < desc->num_loops; loop_it++) {

//  for (tile_it = 0; tile_it < kernel->desc->num_tiles; tile_it++) {
//    printf("[0] ((( tile[%d] = {%d, %d} )))\n", tile_it, kernel->tiles[tile_it].length, kernel->tiles[tile_it].stride);
//  }

    const int loop_idx = desc->loop_desc[loop_it].idx;

    // Length of the loop
    int length = kernel->loops[loop_idx].upper - kernel->loops[loop_idx].lower + 1;
    // Stride of the loop
    int last_tile = desc->loop_desc[loop_it].tile_desc[desc->loop_desc[loop_it].num_tiles-1].idx;
    kernel->tiles[last_tile].stride = kernel->loops[loop_idx].stride;

//  printf("[TILEK] loops[%d] = %d to %d (%d) : %d tiles\n", loop_it, kernel->loops[loop_idx].lower, kernel->loops[loop_idx].upper, length, desc->loop_desc[loop_it].num_tiles);

    int dyn_tile = desc->loop_desc[loop_it].num_tiles;

//  for (tile_it = 0; tile_it < kernel->desc->num_tiles; tile_it++) {
//    printf("[1] ((( tile[%d] = {%d, %d} )))\n", tile_it, kernel->tiles[tile_it].length, kernel->tiles[tile_it].stride);
//  }

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

//    printf("[TILEK]   > #%d : %d by %d\n", tile_idx, kernel->tiles[tile_idx].length, kernel->tiles[tile_idx].stride);

//    kernel->tiles[tile_idx].remain = length % kernel->tiles[tile_idx].stride;

      length = kernel->tiles[tile_idx].stride;
    }

//  for (tile_it = 0; tile_it < kernel->desc->num_tiles; tile_it++) {
//    printf("[2] ((( tile[%d] = {%d, %d} )))\n", tile_it, kernel->tiles[tile_it].length, kernel->tiles[tile_it].stride);
//  }

    if (dyn_tile != desc->loop_desc[loop_it].num_tiles) {
//    printf("[TILEK]    Dynamic: loops[%d].tiles[%d] : #%d\n", loop_idx, dyn_tile, desc->loop_desc[loop_it].tile_desc[dyn_tile].idx);

      int stride = kernel->loops[loop_idx].stride;
      for (tile_it = desc->loop_desc[loop_it].num_tiles - 1; tile_it > dyn_tile; tile_it--) {
        const int tile_idx = desc->loop_desc[loop_it].tile_desc[tile_it].idx;
        const int param = desc->loop_desc[loop_it].tile_desc[tile_it].param;

//      printf("[TILEK]   loops[%d].tiles[%d] : #%d\n", loop_idx, tile_it, tile_idx);

        assert(desc->loop_desc[loop_it].tile_desc[tile_it].kind != e_tile_dynamic);

        if (kernel->tiles[tile_idx].stride == 0)
          kernel->tiles[tile_idx].stride = stride;
        else assert(kernel->tiles[tile_idx].stride == stride);

        if (kernel->tiles[tile_idx].length == 0) {
          kernel->tiles[tile_idx].length = stride * param;
//        kernel->tiles[tile_idx].remain = 0;
        }
        else assert(kernel->tiles[tile_idx].length = stride * param);

//      printf("[TILEK]   > #%d : %d by %d\n", tile_idx, kernel->tiles[tile_idx].length, kernel->tiles[tile_idx].stride);

        stride = kernel->tiles[tile_idx].length;
      }

//    for (tile_it = 0; tile_it < kernel->desc->num_tiles; tile_it++) {
//      printf("[3] ((( tile[%d] = {%d, %d} )))\n", tile_it, kernel->tiles[tile_it].length, kernel->tiles[tile_it].stride);
//    }

      const int tile_idx = desc->loop_desc[loop_it].tile_desc[dyn_tile].idx;
      if (dyn_tile == desc->loop_desc[loop_it].num_tiles - 1) {
        kernel->tiles[tile_idx].stride = kernel->loops[loop_idx].stride;
//      printf("[TILEK] > (DYN-last) loops[%d].tiles[%d] = %d by %d : #%d\n", loop_idx, dyn_tile, kernel->tiles[tile_idx].length, kernel->tiles[tile_idx].stride, tile_idx);
      }
      else if (dyn_tile == 0) {
        assert(desc->loop_desc[loop_it].num_tiles > 1);
        kernel->tiles[tile_idx].stride = (kernel->loops[loop_idx].upper - kernel->loops[loop_idx].lower + 1) / kernel->tiles[desc->loop_desc[loop_it].tile_desc[1].idx].length;
//      printf("[TILEK] > (DYN-first) loops[%d].tiles[%d] = %d by %d : #%d\n", loop_idx, dyn_tile, kernel->tiles[tile_idx].length, kernel->tiles[tile_idx].stride, tile_idx);
      }
      else {
//      assert(desc->loop_desc[loop_it].num_tiles > 2);
//      assert(dyn_tile < desc->loop_desc[loop_it].num_tiles - 1);
//      assert(dyn_tile > 0);
//      kernel->tiles[tile_idx].stride = kernel->tiles[desc->loop_desc[loop_it].tile_desc[dyn_tile - 1].idx].stride / kernel->tiles[desc->loop_desc[loop_it].tile_desc[dyn_tile + 1].idx].length;
        kernel->tiles[tile_idx].stride = stride;
//      printf("[TILEK] > (DYN-middle) loops[%d].tiles[%d] = %d by %d : #%d\n", loop_idx, dyn_tile, kernel->tiles[tile_idx].length, kernel->tiles[tile_idx].stride, tile_idx);
      }
    }

//  for (tile_it = 0; tile_it < kernel->desc->num_tiles; tile_it++) {
//    printf("[4] ((( tile[%d] = {%d, %d} )))\n", tile_it, kernel->tiles[tile_it].length, kernel->tiles[tile_it].stride);
//  }
  }

//printf("\n");

//for (tile_it = 0; tile_it < kernel->desc->num_tiles; tile_it++) {
//  printf("tile[%d] = {%d, %d}\n", tile_it, kernel->tiles[tile_it].length, kernel->tiles[tile_it].stride);
//}

//printf("\n");

  struct context_t context = { kernel->loops, kernel->tiles };
  (*desc->kernel_ptr)(kernel->param, kernel->data, kernel->scalar, &context);
}

