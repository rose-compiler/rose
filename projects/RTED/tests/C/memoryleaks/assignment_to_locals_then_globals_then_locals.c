#include <stdlib.h>


int* global;

int* foo() {
    // enter scope
    int* l1;
    l1 = (int*) malloc( sizeof( int ));
    global = (int*) malloc( sizeof( int ));

    return l1;
}


int main( int argc, char** argv ) {
  int y[ 2 ];
  int* x = foo();

  free( global );

  // l1 is out of scope, so nothing else points to *x, and this assignment
  // leaks;
  x = y;
}
