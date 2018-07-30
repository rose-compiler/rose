#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10

#include "foo2_c.h"

int main()
{
  StartClock();
  int *ptr = (int *)(malloc((100 * (sizeof(int )))));
  int *ptr2 = (int *)(malloc((10 * (sizeof(int )))));
  int *start_ptr = ptr;
  int *start_ptr2 = ptr2;

  // Crossing the boundary of ptr. The condition should
  // be less than, not less than or equal to
  // ptr[PTR_SIZE] is an out-of-bounds access
  for (int index = 0; index <= 100; index++) {
    *ptr = index; // <<< out of bounds access, *ptr[index] with index = 100
    ptr++;
  }
// Resetting ptr to start_ptr, so that it points to the beginning
// of the allocation
  ptr = start_ptr;
// Printing what we wrote above
  for (int index = 0; index <= (100 + 1); index++) {
    printf("ptr[%d]=%d\n",index, *ptr);
    ptr++;
  }
#if 0
// Resetting ptr to start_ptr, so that it points to the beginning
// of the allocation
// Memsetting ptr and ptr2 allocations, in one go.
// This is also crossing the boundaries of ptr. It assumes that
// ptr and ptr2 are in contiguous locations
// Resetting ptr to start_ptr, so that it points to the beginning
// of the allocation
// Printing ptr and ptr2 *and* one more beyond ptr2, all using
// ptr! This still works since malloc asks for more than it needs
// always.
#endif
  printf("Before free ptr2\n");
  fflush(0L);
  free(ptr2);
#if 0
    #if 0
// Retrying the print above, after freeing ptr2. This should
// crash--- and it does!
    #endif
// Allocating another pointer
// This allocation might take the place of ptr2. In this case,
// printing ptr beyond its boundaries should be okay
// Nope this also crashes!
#endif
  EndClock();
  return 0;
}
