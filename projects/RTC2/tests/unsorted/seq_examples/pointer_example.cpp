#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_ACCESS 1

int main() {
    int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));

    

    assert(ptr2 != NULL);

    int* start_ptr = ptr;
    int* start_ptr2 = ptr2;


    // Crossing the boundary of ptr. The condition should
    // be less than, not less than or equal to
    // ptr[PTR_SIZE] is an out-of-bounds access
//    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_ACCESS); index++) {
    for(int index = 0; index < PTR_SIZE; index++) {
        *ptr = index;
        ptr++;
    }

    // Resetting ptr to start_ptr, so that it points to the beginning
    // of the allocation
    ptr = start_ptr;
    // Printing what we wrote above
//    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_ACCESS); index++) {
    for(int index = 0; index < PTR_SIZE; index++) {
        printf("ptr[%d]=%d\n", index, *ptr);
        ptr++;
    }

#if 0
    // Resetting ptr to start_ptr, so that it points to the beginning
    // of the allocation
    ptr = start_ptr;
    // Memsetting ptr and ptr2 allocations, in one go.
    // This is also crossing the boundaries of ptr. It assumes that
    // ptr and ptr2 are in contiguous locations
    memset(ptr, 0, (PTR_SIZE + PTR2_SIZE)*sizeof(int));

    // Resetting ptr to start_ptr, so that it points to the beginning
    // of the allocation
    ptr = start_ptr;
    // Printing ptr and ptr2 *and* one more beyond ptr2, all using 
    // ptr! This still works since malloc asks for more than it needs
    // always.
    for(int index = 0; index <= (PTR_SIZE + PTR2_SIZE); index++) {
        printf("ptr[%d]=%d\n", index, *ptr);
        ptr++;
    }


    for(int index = 0; index < PTR2_SIZE; index++) {
        *ptr2 = index;
        ptr2++;
    }

    ptr2 = start_ptr2;
#endif
    assert(ptr2 != NULL);
    printf("Before free ptr2\n");
    fflush(NULL);
    

    free(ptr2);
    free(start_ptr);

#if 0
    #if 0
    // Retrying the print above, after freeing ptr2. This should
    // crash--- and it does!
    ptr = start_ptr;
    for(int index = 0; index <= (PTR_SIZE + PTR2_SIZE); index++) {
        printf("ptr[%d]=%d\n", index, *ptr);
        ptr++;
    }
    #endif


    printf("after ptr2 freed, before ptr3 alloc\n");
    fflush(NULL);
    // Allocating another pointer
    // This allocation might take the place of ptr2. In this case,
    // printing ptr beyond its boundaries should be okay
    // Nope this also crashes!
    int* ptr3 = (int*)malloc(PTR3_SIZE*sizeof(int));
    ptr = start_ptr;
    for(int index = 0; index <= (PTR_SIZE); index++) {
        printf("ptr[%d]=%d\n", index, *ptr);
        ptr++;
    }
#endif    
    
    return 1;
}
