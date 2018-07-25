#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1

// FIXME: The parameters are not being replaced with
// structed versions
int* fn2(int*, int*, int*, int*); 


int* fn1(int* input) {
    
    int* nullptr = fn2(input, (int*)input, (int*)input, (int*)input);

    printf("input: %u\n", *input);
    return (int*)malloc(PTR_SIZE*sizeof(int));
}

int* fn2(int* input, int* input2, int* input3, int* input4) {
    return NULL;
}

int main() {
    
    int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));

    int* start_ptr = ptr;
    int* start_ptr2 = ptr2;

       int *start_ptr3 = (int*)malloc(PTR_SIZE*sizeof(int)), *start_ptr4 = start_ptr2; 
    
    #if 1
    *start_ptr = 1;
    *start_ptr2 = 1;

    *ptr = 3;
    *ptr2 = 9;
    #endif
    int* new_ptr;

    for(new_ptr = start_ptr; new_ptr < start_ptr + PTR_SIZE; new_ptr++) {
        *new_ptr = 5;
        printf("%u\n", *new_ptr);
    }

    int* whileptr = NULL;

    do {    
        int* doptr = start_ptr, *forptr2, *doptr2;
        for(forptr2 = (int*)malloc(PTR_SIZE*sizeof(int)), doptr2 = forptr2; doptr2 < (forptr2 + PTR_SIZE) ; doptr2++) {
        }

    }while(whileptr != NULL);

    int* tempptr = start_ptr;

    if(whileptr == NULL) {
        start_ptr += PTR_SIZE - 1;
        *start_ptr = 10;
    }

    start_ptr = tempptr;

    printf("Final print\n");
    for(new_ptr = start_ptr; new_ptr < start_ptr + PTR_SIZE; new_ptr++) {
        printf("%u\n", *new_ptr);
    }
    printf("Final print -end\n");


    // CIRM Review Code
    int *p = (int*)malloc(PTR_SIZE*sizeof(int));
    int *q;
    int y = (q = p, *q = 5, *q);
    // ------



    // Passing pointers to function
    int* fnptr = fn1(start_ptr);
    int* fnptr2;
    fnptr2 = fn1(fnptr);
    // ------

    // Complex expressions
    int* complexptr = start_ptr + 1;
    int complexexp = *ptr + *(fn1(start_ptr++)) - *p;
    // Add more



    return 1;
}
