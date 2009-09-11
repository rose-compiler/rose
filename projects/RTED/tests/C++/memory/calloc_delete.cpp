#include <stdlib.h>


int main() {

    int* x;
    x = (int*) calloc( 2, sizeof( int ));

    x[ 0 ] = 10;

    // should use free with malloc
    delete x;

    return 0;
}
