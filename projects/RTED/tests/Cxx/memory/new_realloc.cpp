#include <stdlib.h>


int main() {

    int* x;
    int* backup;

    x = new int;

    // avoid memory leak so that we can expose the interesting error
    backup = x;

    // should not use realloc with memory allocated via new
    x = (int*) realloc( x, sizeof( int ));

    free( x );

    return 0;
}
