
#define TILEK_STATIC 1

#include "TileK/loop.h"
#include "TileK/kernel.h"
#include "TileK/tile.h"

extern kernel_func_ptr kernel_0;

struct tile_desc_t tiles_loop_1[2] = {
  { 0, e_tile_static,   2 },
  { 1, e_tile_dynamic,  0 }
};

struct tile_desc_t tiles_loop_2[2] = {
  { 2, e_tile_static,   2 },
  { 3, e_tile_dynamic,  0 }
};

struct loop_desc_t loop_desc[2] = {
  { 0, 2, tiles_loop_1 },
  { 1, 2, tiles_loop_2 }
};

struct kernel_desc_t kernel_desc[1] = {
  { 2, 2, 2, 4, loop_desc, &kernel_0 }
};

