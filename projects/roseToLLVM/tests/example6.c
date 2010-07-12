#include <stdio.h>
#include <stdlib.h>

//
// Test storage allocation and storage deallocation
//
int main(int argc, char *argv[]) {
    int *arr = (int *) malloc(5 * sizeof(int));

    for (int i = 0; i < 5; i++) {
        arr[i] = i;
    }

    for (int k = 0; k < 5; k++)
        printf("arr[%i] = %i\n", k, arr[k]);

    free(arr);

    return 0;
}
