#include <stdlib.h>

typedef struct {
	char before;
	int arr[ 3 ];
	int after;
} type;

int main( int argc, char **argv ) {

	type *x;
	int *y;

	x = (type*) malloc( sizeof( type ));

	x -> arr[ 0 ] = 0;
	x -> arr[ 1 ] = 1;
	x -> arr[ 2 ] = 2;
	x -> after = 3;

	// this is legal
	y = &( x -> after );


	y = &( x -> arr[ 2 ]);
	// we should detect that y has gone out-of-bounds of x[ 0 ], even though it
	// still points to an int and still to the same memory region
	y++;

	return 0;
}

