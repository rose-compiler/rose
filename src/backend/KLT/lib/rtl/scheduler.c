
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/scheduler.h"

#include <assert.h>

void klt_schedule(int num_loops, struct loop_desc_t * loop_desc, struct loop_t * loops, struct tile_t * tiles) {
  int loop_it, tile_it;

  for (loop_it = 0; loop_it < num_loops; loop_it++) {
    if (loop_desc[loop_it].num_tiles == 0) continue;

    const int loop_idx = loop_desc[loop_it].idx;

    int length = loops[loop_idx].upper - loops[loop_idx].lower + 1;               // Length of the loop
    int last_tile = loop_desc[loop_it].tile_desc[loop_desc[loop_it].num_tiles-1].idx; // Stride of the loop
    tiles[last_tile].stride = loops[loop_idx].stride;

    int dyn_tile = loop_desc[loop_it].num_tiles;

    for (tile_it = 0; tile_it < loop_desc[loop_it].num_tiles; tile_it++) {
      const int tile_idx = loop_desc[loop_it].tile_desc[tile_it].idx;
      const int param = loop_desc[loop_it].tile_desc[tile_it].param;

      tiles[tile_idx].length = length;

      if (loop_desc[loop_it].tile_desc[tile_it].kind == e_tile_dynamic) {
        dyn_tile = tile_it;
        break;
      }

      if (tiles[tile_idx].stride == 0)
         tiles[tile_idx].stride = length / param;
      else assert(tiles[tile_idx].stride == length / param);

      assert(tiles[tile_idx].stride > 0);

      //tiles[tile_idx].remain = length % tiles[tile_idx].stride;

      length = tiles[tile_idx].stride;
    }

    if (dyn_tile != loop_desc[loop_it].num_tiles) {
      int stride = loops[loop_idx].stride;
      for (tile_it = loop_desc[loop_it].num_tiles - 1; tile_it > dyn_tile; tile_it--) {
        const int tile_idx = loop_desc[loop_it].tile_desc[tile_it].idx;
        const int param = loop_desc[loop_it].tile_desc[tile_it].param;

        assert(loop_desc[loop_it].tile_desc[tile_it].kind != e_tile_dynamic);

        if (tiles[tile_idx].stride == 0)
          tiles[tile_idx].stride = stride;
        else assert(tiles[tile_idx].stride == stride);

        if (tiles[tile_idx].length == 0) {
          tiles[tile_idx].length = stride * param;
          //tiles[tile_idx].remain = 0;
        }
        else assert(tiles[tile_idx].length = stride * param);

        stride = tiles[tile_idx].length;
      }

      const int tile_idx = loop_desc[loop_it].tile_desc[dyn_tile].idx;
      if (dyn_tile == loop_desc[loop_it].num_tiles - 1) {
        tiles[tile_idx].stride = loops[loop_idx].stride;
      }
      else if (dyn_tile == 0) {
        assert(loop_desc[loop_it].num_tiles > 1);
        tiles[tile_idx].stride = (loops[loop_idx].upper - loops[loop_idx].lower + 1) / tiles[loop_desc[loop_it].tile_desc[1].idx].length;
      }
      else tiles[tile_idx].stride = stride;
    }
  }
}

