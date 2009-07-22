#include <stdlib.h>

int main( int argc, char **argv ) {

	int *x, *p;
	int y;

	x = (int*) malloc( 3 * sizeof( int ));
	p = (int*) malloc( 3 * sizeof( int ));
	x[ 0 ] = 0;
	x[ 1 ] = 1;
	x[ 2 ] = 2;
	
	// we should detect that x[ 4 ] is out of bounds, even if it's legal memory
	// (e..g in &p)
	y = x[ 4 ];

	return 0;
}

