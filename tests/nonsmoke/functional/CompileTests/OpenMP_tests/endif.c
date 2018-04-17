/*
 *  test the handling of #endif at the end of a parallel region
 *  Extracted from BOTS
 * Liao 1/15/2009
 * */
#include <stdio.h>
#define FORCE_TIED_TASKS
void find_queens (int size)
{
  int total_count=0;
#pragma omp parallel
  {
#ifdef FORCE_TIED_TASKS
#pragma omp atomic
    total_count += 1;
#endif
    // printf("aa");
  }
}
