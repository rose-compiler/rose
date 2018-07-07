#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1

typedef struct UserStruct{
    unsigned int *ptr1;
    unsigned int *ptr2;
    unsigned int var1;
    char var2;
    float var3;
    float* ptr3;
    unsigned int *ptr4;
}UserStruct;

int main() {
    int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));

    UserStruct structvar = {NULL, (unsigned int*)ptr, 5, 'a', 3.5, (float*)ptr2, NULL};
    UserStruct **structptr = (UserStruct**)malloc(PTR_SIZE*sizeof(UserStruct*));
    for(unsigned int index = 0; index < PTR_SIZE; index++) {
        structptr[index] = (UserStruct*)malloc(PTR2_SIZE*sizeof(UserStruct));
    }

    for(unsigned int i = 0; i < PTR_SIZE; i++) {
        for(unsigned int j = 0; j < PTR2_SIZE; j++) {
            structptr[i][j].ptr1= (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int));
            structptr[i][j].ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(unsigned int));
            structptr[i][j].ptr3 = (float*)malloc((PTR_SIZE+100)*sizeof(float));
            structptr[i][j].ptr4 = (unsigned int*)malloc((PTR2_SIZE+50)*sizeof(unsigned int));
        }
    }

    for(unsigned int i = 0; i < PTR_SIZE; i++) {
        for(unsigned int j = 0; j < PTR2_SIZE; j++) {
            structptr[i][j].ptr1 = NULL;
            structptr[i][j].ptr2 = NULL;
        }
    }


    int* start_ptr = ptr;
    int* start_ptr2 = ptr2;

    // Crossing the boundary of ptr. The condition should
    // be less than, not less than or equal to
    // ptr[PTR_SIZE] is an out-of-bounds access
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {
        *ptr = index;
        ptr++;
    }


    for(unsigned int i = 0; i < PTR_SIZE; i++) {
        for(unsigned int j = 0; j < PTR2_SIZE; j++) {
            printf("%lf\n", structptr[i][j].ptr3);
            printf("%u\n", structptr[i][j].ptr4);
        }
    }

    // Resetting ptr to start_ptr, so that it points to the beginning
    // of the allocation
    ptr = start_ptr;
    // Printing what we wrote above
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {
        printf("ptr[%d]=%d\n", index, *ptr);
        ptr++;
    }
    
    return 0;
}
