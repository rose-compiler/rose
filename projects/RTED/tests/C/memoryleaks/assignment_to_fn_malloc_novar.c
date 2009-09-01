#include <stdlib.h>


int* foo() {
	return (int*) malloc( sizeof( int ));
}

int main( int argc, char** argv) {
	int x[ 2 ];
	int* a;

	a = foo();
	// error, did not free memory malloc-d by foo
	a = x;
	free( a );  // actually this is also bad, trying to free stack var

	return 0;
}

