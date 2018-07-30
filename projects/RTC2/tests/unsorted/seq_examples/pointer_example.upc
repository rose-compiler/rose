#include <upc.h> /* Required for UPC extensions */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//shared int* shared ptr; // shared pointer to shared data
shared int* ptr; // local pointer to shared data

int main() {

    #if 0
    if(MYTHREAD == 0) {
        ptr = upc_global_alloc(THREADS, sizeof(int));
    }
    #endif

    #if 1
    ptr = upc_all_alloc(THREADS, sizeof(int)); // Each thread gets one int
    #endif
    upc_barrier;
    
    *ptr = MYTHREAD;
    printf("Calling free on %d\n", MYTHREAD);
    upc_free(ptr);
    upc_barrier;
    printf("MYTHREAD: %d, *ptr: %d\n", MYTHREAD, *ptr);

    #if 0
    if(MYTHREAD == 0) {
        printf("Calling free on %d\n", MYTHREAD);
        upc_free(ptr);
        printf("MYTHREAD: %d, *ptr: %d\n", MYTHREAD, *ptr);
    }
    else {
        printf("MYTHREAD: %d, *ptr: %d\n", MYTHREAD, *ptr);
    }
    #endif

    return 1;
}
