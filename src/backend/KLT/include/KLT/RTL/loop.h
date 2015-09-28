
#ifndef __KLT_RTL_LOOP_H__
#define __KLT_RTL_LOOP_H__

struct klt_loop_container_t {
  int num_loops;
  int num_tiles;
  struct klt_loop_desc_t * loop_desc;
};

struct klt_loop_desc_t {
  int idx;
  int num_tiles;
  struct klt_tile_desc_t * tile_desc;
};

struct klt_loop_t {
  int lower;
  int upper;
  int stride;
};

#endif /* __KLT_RTL_LOOP_H__ */

