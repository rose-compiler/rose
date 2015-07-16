
#ifndef __KLT_RTL_TILE_H__
#define __KLT_RTL_TILE_H__

struct tile_desc_t {
  int idx;
  enum tile_kind_e {
    e_tile_static = 0,
    e_tile_dynamic = 1
  } kind;
  int param;
};

struct tile_t {
  int length;
  int stride;
//int remain;
};

#endif /* __KLT_RTL_TILE_H__ */

