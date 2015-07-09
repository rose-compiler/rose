
#include "TileK/defs.h"

#ifndef __TILEK_KERNEL_H__
#define __TILEK_KERNEL_H__

struct loop_t;
struct tile_t;

struct context_t {
  struct loop_t * loops;
  struct tile_t * tiles;
};

typedef void (*kernel_func_ptr)(int *, void **, void **,  struct context_t *);

struct kernel_desc_t {
  int num_data;
  int num_param;
  int num_scalar;
  int num_loops;
  int num_tiles;
  struct loop_desc_t * loop_desc;
  kernel_func_ptr kernel_ptr;
};

extern struct kernel_desc_t kernel_desc[];

struct kernel_t {
  struct kernel_desc_t * desc;
  void ** data;
  int   * param;
  void ** scalar;
  struct loop_t * loops;
  struct tile_t * tiles; // filled by runtime
};


struct kernel_t * build_kernel(int idx);

void execute_kernel(struct kernel_t * kernel);

#endif /* __KERNEL_H__ */

