#include <stdlib.h>


int main( int argc, char **argv ) {

    struct type {
        char before;
        int arr[ 3 ];
        int after;
    }* x;
	int *y;

    x = (struct type*) malloc( sizeof( struct type ));

	x -> arr[ 0 ] = 0;
	x -> arr[ 1 ] = 1;
	x -> arr[ 2 ] = 2;
	x -> after = 3;

	// this is legal
	y = &( x -> after );


	y = &( x -> arr[ 2 ]);
	// we should detect that y has gone out-of-bounds of x->arr, even though it
	// still points to an int and still to the same memory region
	y++;

	return 0;
}

