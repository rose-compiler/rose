#include <stdlib.h>


int main( int argc, char** argv ) {

  int z[ 2 ];
  int* x;

  int i = 1;
  switch( --i ) {
    int* y;
    case 0:
        y = (int*) malloc( sizeof( int ));
        x = y;
      break;
    default:
      break;
  }

  // error, y is out of scope so nothing else points to that memory
  x = z;

	return 0;
}

