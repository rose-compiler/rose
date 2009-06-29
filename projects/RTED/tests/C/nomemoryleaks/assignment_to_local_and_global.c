#include <stdlib.h>


int* global;

void foo() {
    // enter scope
    int* l1 = (int*) malloc(sizeof(int));
    global = (int*) malloc(sizeof(int));

    free( l1);
}


int main( int argc, char* argv) {
  // this should be fine, as global does not go out of scope after the function
  // call
  foo();
  free( global);
}
