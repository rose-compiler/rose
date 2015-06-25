
#include "TileK/defs.h"

#ifndef __TILEK_LOOP_H__
#define __TILEK_LOOP_H__

struct loop_desc_t {
  int idx;
  int num_tiles;
  struct tile_desc_t * tile_desc;
};

struct loop_t {
  int lower;
  int upper;
  int stride;
};

#endif /* __TILEK_LOOP_H__ */

