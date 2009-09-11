#include <stdlib.h>


int main() {

    int* x;
    x = new int;

    // should use delete with new
    free( x );

    return 0;
}
