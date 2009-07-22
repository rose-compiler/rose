#include <stdlib.h>

typedef struct {
	char before;
	int arr[ 3 ];
	int after;
} TypeA;

typedef struct {
	char before;
	int arr[ 3 ];
	int after;
} TypeB;

int main( int argc, char **argv ) {

	TypeA *x;
	TypeB *p;
	int *y;

	x = (TypeA*) malloc( 2 * sizeof( TypeA ) + sizeof( TypeB ));
	p = (TypeB*) (x + 2);

	x -> arr[ 0 ] = 0;
	x -> arr[ 1 ] = 1;
	x -> arr[ 2 ] = 2;
	x -> after = 3;

	x++;
	x -> arr[ 0 ] = 0;
	x -> arr[ 1 ] = 1;
	x -> arr[ 2 ] = 2;
	x -> after = 3;

	x--;

	p -> arr[ 0 ] = 0;
	p -> arr[ 1 ] = 1;
	p -> arr[ 2 ] = 2;


	y = &( x[ 1 ].arr[ 1 ]);
	size_t diff = (size_t) &( p -> arr[ 1 ]) - (size_t) y;
	// this should be illegal even if the result winds up in an int[ 3 ] since
	// we crossed over from a TypeA array into a TypeB array.
	y += ( diff / sizeof( int ));


	// this is legal
	y = &( x -> after );

	y = &( x -> arr[ 2 ]);
	// we should detect that y has gone out-of-bounds of x[ 0 ], even though it
	// still points to an int and still to the same memory region
	y++;

	return 0;
}

