#include <stdlib.h>


int main( int argc, char* argv) {

  int z[2];
  int* x;

  do  {
    int* y = (int*) malloc( sizeof(int));
    x = y;
  } while ( false);

  // error, y is out of scope so nothing else points to that memory
  x = z;

	return 0;
}

