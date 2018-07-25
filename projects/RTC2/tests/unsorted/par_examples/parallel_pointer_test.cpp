#include <cstdio>
#include <cstdlib>

#define ARR_SIZE 100

int main() {
    int* arr[100];
    for(int index = 0; index < ARR_SIZE; index++) {
        arr[index] = (int*)malloc(sizeof(int)*ARR_SIZE);
    }

    for(int index = 0; index < ARR_SIZE; index++) {
        free(arr[index]);
    }
    
    return 0;
}
