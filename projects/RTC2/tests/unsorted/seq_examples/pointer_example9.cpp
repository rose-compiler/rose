#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1

unsigned int* fn1(unsigned int* input) {
    
    printf("input: %u\n", *input);
    printf("input: %u\n", *(input++));
    return (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int));
}


int main() {
    
    unsigned int *ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
    unsigned int *ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));

    unsigned int* start_ptr = ptr;
    unsigned int* start_ptr2 = ptr2;

    unsigned int *start_ptr3 = (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int)), *start_ptr4 = start_ptr2; 
    

    unsigned int* ptr5 = fn1(start_ptr3);

    for(unsigned int* temp = ptr5; temp < ptr5 + PTR_SIZE; temp++) {
        printf("%u\n", *temp);
    }

    return 1;
}
