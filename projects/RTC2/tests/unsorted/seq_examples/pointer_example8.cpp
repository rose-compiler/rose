#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1

unsigned int* fn1(unsigned int* input, unsigned int* input2) {
    
    printf("input: %u\n", *input);
    return (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int));
}


int main() {
    
    unsigned int *ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
    unsigned int *ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));

    unsigned int* start_ptr = ptr;
    unsigned int* start_ptr2 = ptr2;

    unsigned int *start_ptr3 = (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int)), *start_ptr4 = start_ptr2; 
    
    #if 0
    #if 1
    *start_ptr = 1;
    *start_ptr2 = 1;

    *ptr = 3;
    *ptr2 = 9;
    #endif

    for(unsigned int* new_ptr = start_ptr; new_ptr < start_ptr + PTR_SIZE; new_ptr++) {
        *new_ptr = 5;
        printf("%u\n", *new_ptr);
    }

    unsigned int* whileptr = NULL;

    do {    
        unsigned int* doptr = start_ptr;
        for(unsigned int* forptr2 = (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int)), *doptr2 = forptr2; doptr2 < (forptr2 + PTR_SIZE) ; doptr2++) {
        }

    }while(whileptr != NULL);

    unsigned int* tempptr = start_ptr;

    if(whileptr == NULL) {
        start_ptr += PTR_SIZE - 1;
        *start_ptr = 10;
    }

    start_ptr = tempptr;

    printf("Final print\n");
    for(unsigned int* new_ptr = start_ptr; new_ptr < start_ptr + PTR_SIZE; new_ptr++) {
        printf("%u\n", *new_ptr);
    }
    printf("Final print -end\n");


    // CIRM Review Code
    unsigned int *p = (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int));
    unsigned int *q;
    unsigned int y = (q = p, *q = 5, *q);
    // ------



    // Passing pointers to function
    unsigned int* fnptr = fn1(start_ptr);
    unsigned int* fnptr2;
    fnptr2 = fn1(fnptr);
    // ------

    // Complex expressions
    unsigned int* complexptr = start_ptr + 1;
    unsigned int complexexp = *ptr + *(fn1(start_ptr++)) - *p;
    // Add more

    #endif

    return 1;
}
