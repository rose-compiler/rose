
#define TILEK_KERNEL 1

#include "TileK/kernel.h"
#include "TileK/loop.h"
#include "TileK/tile.h"

#include <stdio.h>

void kernel_0(int * param, void ** data, struct context_t * ctx) {
/*  
  printf("param = {%d , %d}\n", param[0], param[1]);

  printf("loops[0] = %d to %d\n", loops[0].lower, loops[0].upper);
  printf("  tile[0] = %d by %d\n", tiles[0].length, tiles[0].stride);
  printf("  tile[1] = %d by %d\n", tiles[1].length, tiles[1].stride);
  printf("loops[1] = %d to %d\n", loops[1].lower, loops[1].upper);
  printf("  tile[2] = %d by %d\n", tiles[2].length, tiles[2].stride);
  printf("  tile[3] = %d by %d\n", tiles[3].length, tiles[3].stride);
*/
  int it_0_0, it_0_1, it_1_2, it_1_3;
  for (it_0_0 = ctx->loops[0].lower; it_0_0 < ctx->loops[0].lower + ctx->tiles[0].length; it_0_0 += ctx->tiles[0].stride) {
    for (it_1_2 = ctx->loops[1].lower; it_1_2 < ctx->loops[1].lower + ctx->tiles[2].length; it_1_2 += ctx->tiles[2].stride) {
      for (it_0_1 = it_0_0; it_0_1 < it_0_0 + ctx->tiles[1].length; it_0_1 += ctx->tiles[1].stride) {
        for (it_1_3 = it_1_2; it_1_3 < it_1_2 + ctx->tiles[3].length; it_1_3 += ctx->tiles[3].stride) {
//        printf("  (%d , %d)\n", it_0_1, it_1_3);
//        *get_data_A(data, param, it_0_1, it_1_3) += get_data_b(data, param);
          ((float *)data[0])[it_0_1 * param[1] + it_1_3] += *((float *)data[1]);
        }
      }
    }
  }
}

#if 0
void kernel_0(void ** data, int * param, struct context_t * ctx) {
/*  
  printf("param = {%d , %d}\n", param[0], param[1]);

  printf("loops[0] = %d to %d\n", loops[0].lower, loops[0].upper);
  printf("  tile[0] = %d by %d\n", tiles[0].length, tiles[0].stride);
  printf("  tile[1] = %d by %d\n", tiles[1].length, tiles[1].stride);
  printf("loops[1] = %d to %d\n", loops[1].lower, loops[1].upper);
  printf("  tile[2] = %d by %d\n", tiles[2].length, tiles[2].stride);
  printf("  tile[3] = %d by %d\n", tiles[3].length, tiles[3].stride);
*/
  int it_0_0, it_0_1, it_1_2, it_1_3;
  for (it_0_0 = 0; it_0_0 < ctx->tiles[0].length; it_0_0 += ctx->tiles[0].stride) {
    for (it_1_2 = 0; it_1_2 < ctx->tiles[2].length; it_1_2 += ctx->tiles[2].stride) {
      for (it_0_1 = 0; it_0_1 < ctx->tiles[1].length; it_0_1 += ctx->tiles[1].stride) {
        for (it_1_3 = 0; it_1_3 < ctx->tiles[3].length; it_1_3 += ctx->tiles[3].stride) {

          int it_0 = it_0_1 + it_0_0 + ctx->loops[0].lower;
          int it_1 = it_1_3 + it_1_2 + ctx->loops[1].lower;
//        printf("  (%d , %d)\n", it_0_1, it_1_3);
//        *get_data_A(data, param, it_0_1, it_1_3) += get_data_b(data, param);
          ((float *)data[0])[it_0 * param[1] + it_1] += *((float *)data[1]);
        }
      }
    }
  }
}
#endif

