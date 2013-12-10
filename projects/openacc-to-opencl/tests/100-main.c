/*!
 * \addtogroup grp_libopenacc_tests
 * @{
 *
 * \file tests/test_01.c
 *
 */

#include <math.h>

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#ifdef OPENACC
#include "OpenACC/openacc.h"
#endif

#ifdef OPENACC_TIMER
#include "OpenACC/utils/timer.h"
#endif

#include KERNEL_FILE

#include INIT_FILE

void read_args(
  int argc, char ** argv,
  unsigned long * num_gang,
  unsigned long * num_worker,
  unsigned long * vector_length
) {
  assert(argc >= 4);
  *num_gang = atoi(argv[1]);
  *num_worker = atoi(argv[2]);
  *vector_length = atoi(argv[3]);
}

int main(int argc, char ** argv) {
  unsigned long num_gang;
  unsigned long num_worker;
  unsigned long vector_length;

  acc_timer_t data_timer = acc_timer_build();
  acc_timer_t comp_timer = acc_timer_build();

  read_args(argc, argv, &num_gang, &num_worker, &vector_length);

  launch(argc, argv, num_gang, num_worker, vector_length, data_timer, comp_timer);

  // CSV format : computation time, computation + communication time
  printf("%d,%d", comp_timer->delta, data_timer->delta);

  return 0;
}

/*! @} */

