#include <stdlib.h>


int* baz() {
	int* y;
  y = (int*) malloc( sizeof( int ));
	return y;
}

int* bar() {
	return baz();
}

int* foo() {
	return bar();
}

int main( int argc, char** argv ) {
	int x[ 2 ];
	int* a = foo();

	// error, did not free memory malloc-d by foo (via bar, baz)
	a = x;

	return 0;
}

