#include <upc.h> /* Required for UPC extensions */
#include <stdio.h>
#include <pthread.h>

void* pthread_hello_world(void* m) {
    printf("%s: hello_world\n", (char*)m);
    return NULL;
}

int main() {
    pthread_t tx;
    #if 0
    if(MYTHREAD == 0) {
        pthread_create(&tx, NULL, pthread_hello_world, (void*)"ThreadX");
    }
    #endif

    printf("Hello from %d of %d\n", MYTHREAD, THREADS);
    upc_barrier;
    pthread_exit(NULL);
    return 1;
}
