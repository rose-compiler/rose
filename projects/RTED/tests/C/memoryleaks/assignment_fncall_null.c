#include <stdlib.h>


int* foo() {
	int* y = (int*) malloc(sizeof(int));
  return y;
}

int main( int argc, char* argv) {

	// error, did not free or assign memory malloc-d by foo
	foo();

	return 0;
}

