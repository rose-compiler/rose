#include <upc.h> /* Required for UPC extensions */
#include <stdio.h>

int main() {
    printf("Hello from %d of %d\n", MYTHREAD, THREADS);
    return 1;
}
