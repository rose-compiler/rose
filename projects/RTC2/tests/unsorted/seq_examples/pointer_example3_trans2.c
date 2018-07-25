#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ptr_hdr.h"

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1

int main() {
        
    struct IntStruct ptr = {(int*)malloc(PTR_SIZE*sizeof(int))};
    struct IntStruct ptr2 = {(int*)malloc(PTR2_SIZE*sizeof(int))};

    struct IntStruct start_ptr = ptr;
    struct IntStruct start_ptr2 = ptr2;

    struct IntStruct start_ptr3 = {(int*)malloc(PTR_SIZE*sizeof(int))}, start_ptr4 = start_ptr2; 

    #if 0
    *start_ptr = 1;
    *start_ptr2 = 1;

    *ptr = 3;
    *ptr2 = 9;
    #endif

    return 1;
}
