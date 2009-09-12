#include <stdlib.h>


int main() {

    int* x;
    x = (int*) malloc( sizeof( int ));

    // should use free with malloc
    delete x;

    return 0;
}
