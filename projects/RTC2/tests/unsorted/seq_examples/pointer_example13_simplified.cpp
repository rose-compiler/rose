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

int main() {
       
    unsigned int *no_init_ptr;

    //unsigned int *ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
    unsigned int *ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));

    unsigned int array1[PTR_SIZE];
    unsigned int array2[PTR2_SIZE];

    unsigned int *array_ptr[PTR_SIZE];

    unsigned int array_2d[PTR_SIZE][PTR2_SIZE];

    array1[25] = array2[5]++;

    return 1;
}
