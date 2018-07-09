#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10





int main() {

    unsigned int *no_init_ptr;

    unsigned int *ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
    unsigned int *ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));

    unsigned int* ptr_index;
    unsigned int counter = 0;
    for(ptr_index = ptr + (PTR_SIZE-1); ptr_index >= ptr ; ptr_index--) {
        *ptr_index = counter++;
    }

    for(ptr_index = ptr + PTR_SIZE - 1; ptr_index >= ptr; ptr_index--) {
        printf("%u\n", *ptr_index);
    }

  free(ptr);
  free(ptr2);
    return 0;
}
