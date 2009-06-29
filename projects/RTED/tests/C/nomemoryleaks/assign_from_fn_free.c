#include <stdlib.h>


int* foo() {
	int* y = (int*) malloc(sizeof(int));
  return y;
}

int main( int argc, char* argv) {

  int* x = foo();
  free( x);

	return 0;
}

