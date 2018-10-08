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

struct node {
    struct node* prev;
    struct node* next;
    unsigned int val;
};


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
       
    unsigned int *ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
    unsigned int *ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));

    unsigned int array1[PTR_SIZE];
    unsigned int array2[PTR2_SIZE];

    unsigned int* start_ptr = ptr;
    unsigned int index;
    for(index = 0; index < PTR_SIZE; index++, start_ptr++) {
        array1[index] = index;
        *start_ptr = index;
    }


    struct node stack_node;

    struct node* head = new struct node;

    head->prev = NULL;
    head->next = NULL;

    start_ptr = array1;

    unsigned int counter = 0;

    for(index = 0; index < PTR_SIZE; index++, start_ptr++) {
        counter += *start_ptr;
        printf("array1[index]: %u\n", *start_ptr);
        printf("ptr: %u\n", ptr[index]);
    }
    

    printf("%u\n", counter); 


    return 1;
}
