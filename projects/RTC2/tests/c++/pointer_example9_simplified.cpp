#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1
#define ARRAY_SIZE 29
#define ARRAY2_SIZE 49

void print_array(unsigned int* array, unsigned int size);

// FIXME: The parameters are not being replaced with
// structed versions
unsigned int* fn2(unsigned int*, float*, double*, char*); 


unsigned int* fn1(unsigned int* input) {
    
    unsigned int* nullptr = fn2(input, (float*)input, (double*)input, (char*)input);

    printf("input: %u\n", *input);
    return (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int));
}

unsigned int* fn2(unsigned int* input, float* input2, double* input3, char* input4) {
    return NULL;
}

int main() {
       
    printf("main - begin\n");

    unsigned int *ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
    unsigned int *ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));

    // Added code
    unsigned int int_array1[ARRAY_SIZE];

    *int_array1 = ARRAY_SIZE;
    int* int_array_index;
    printf("Input array\n");
    for(int_array_index = (int*)int_array1 + 1; int_array_index < (int*)int_array1 + ARRAY_SIZE; int_array_index++) {
        *int_array_index = *(int_array_index - 1) + 1;
        printf("%u\n", *int_array_index);
    }
    printf("Done\n");
    
    printf("External printing\n");
    print_array(int_array1, ARRAY_SIZE);
    
    printf("Internal printing\n");
    int index2;
    for(index2 = 0; index2 < ARRAY_SIZE; index2++) {
        printf("%u\n", int_array1[index2]);
    }
    printf("Done\n");

    float float_array2[ARRAY2_SIZE];

    int_array_index = (int*)float_array2;

    printf("sizeof(float): %lu\n", sizeof(float));

    index2 = 0;
    for(;int_array_index < (int*)float_array2 + ARRAY2_SIZE; int_array_index = (int*)(((float*)int_array_index) + 1), index2++) {
        *((float*)int_array_index) = ARRAY2_SIZE + 1;
        printf("%f\n", float_array2[index2]);
        printf("%llu\n", reinterpret_cast<unsigned long long>(int_array_index));
    }

    float* float_index = &float_array2[ARRAY2_SIZE-1];
    (*float_index)++;
    printf("random location:%f\n", float_array2[ARRAY2_SIZE-1]);

    int stack_var = 5;
    
    int* stack_ptr = &stack_var;

    (*stack_ptr)--;

    printf("stack_var: %u, stack_ptr: %u\n", stack_var, *(stack_ptr));
    #if 0
    int int_array_2d[ARRAY_SIZE][ARRAY2_SIZE];

    printf("Input 2D array - begin\n");
    for(index2 = 0; index2 < ARRAY_SIZE; index2++) {
        
        int_array_index = int_array_2d[index2];
        int index3;

        for(index3 = 0; index3 < ARRAY2_SIZE; index3++) {
            
            int_array_index[index3] = index3;
            printf("%u\n", int_array_index[index3]);
        }
    }
    printf("Input 2D array - done\n");

    printf("External printing\n");
    for(index2 = 0; index2 < ARRAY_SIZE; index2++) {    
        int_array_index = int_array_2d[index2];
        print_array((unsigned int*)int_array_index, ARRAY2_SIZE);
    }


    printf("Printing 2D array - begin\n");
    for(index2 = 0; index2 < ARRAY_SIZE; index2++) {
        int index3;
        for(index3 = 0; index3 < ARRAY2_SIZE; index3++) {
            printf("%u\n", int_array_2d[index2][index3]);    
        }
    }
    printf("Printing 2D array - done\n");
    #endif
    // end added code

    printf("ptr & ptr2 - done\n");

    unsigned int* start_ptr = ptr;
    unsigned int* start_ptr2 = ptr2;

    printf("start_ptr & start_ptr2 - done\n");

    unsigned int *start_ptr3 = (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int)), *start_ptr4 = start_ptr2; 

    printf("start_ptr3 - done\n");
    
    #if 1
    *start_ptr = 1;
    *start_ptr2 = 1;

    printf("start_ptr: %u\n", *start_ptr);
    printf("start_ptr2: %u\n", *start_ptr2);

    *ptr = 3;
    *ptr2 = 9;

    printf("start_ptr: %u\n", *start_ptr);
    printf("start_ptr2: %u\n", *start_ptr2);
    #endif

    printf("start_ptr, start_ptr2, ptr, ptr2 - done\n");

    unsigned int* new_ptr;

    for(new_ptr = start_ptr; new_ptr < start_ptr + PTR_SIZE; new_ptr++) {
        *new_ptr = 5;
        printf("%u\n", *new_ptr);
    }

    for(new_ptr = start_ptr2; new_ptr < start_ptr2 + PTR2_SIZE; new_ptr++) {
        *new_ptr = 25;
        printf("%u\n", *new_ptr);
    }

    printf("for loop 1 - done\n");

    unsigned int* whileptr = NULL;

    do {    
        unsigned int* doptr = start_ptr;
        unsigned int* forptr2 = (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int)), *doptr2 = forptr2; 
        for(; doptr2 < (forptr2 + PTR_SIZE) ; doptr2++) {
        }

    }while(whileptr != NULL);

    unsigned int* tempptr = start_ptr;

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
    unsigned int *p = (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int));
    unsigned int* index = p;
    for(; index < p + PTR_SIZE; index++) {
        *index = 15;
        printf("%u\n", *index);
    }

    unsigned int *q;
    unsigned int y = (q = p, *q = 5, *q);

    printf("q: %u\n", *q);
    printf("p: %u\n", *p);
    printf("y: %u\n", y);
    // ------



    // Passing pointers to function
    unsigned int* fnptr = fn1(start_ptr);
    unsigned int* fnptr2;
    fnptr2 = fn1(fnptr);
    // ------

    // Complex expressions
    unsigned int* complexptr = start_ptr + 1;
    unsigned int complexexp = *ptr + *(fn1(start_ptr++)) - *p;
    printf("complexptr: %u\n", *complexptr);
    printf("complexexp: %u\n", complexexp);
    // Add more

    print_array(p, PTR_SIZE);
    print_array(ptr, PTR_SIZE);
    print_array(ptr2, PTR2_SIZE);

    return 1;
}
