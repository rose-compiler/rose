#include <stdlib.h>


int main( int argc, char* argv) {

  int z[2];
  int* x;

  int i = 1;
  while( --i >= 0) {
    int* y = (int*) malloc( sizeof(int));
    x = y;
  }

  // error, y is out of scope so nothing else points to that memory
  x = z;

	return 0;
}

