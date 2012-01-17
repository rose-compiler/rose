#include <stdlib.h>

int* baz() {
  // \pp the November'10 RTED tests reported this memory leak
  //     at the correct line, for the wrong reason.
  //     That is, it wrapped main's "int* a = foo()"
  //     in beginScope()/endScope(); The variable a disappeared
  //     and the write "a=x" was unsuccessful (b/c variable a was gone).
  //     Thus I modified the malloc call to a version that RTED Jul'11
  //     can properly instrument (that is mallocs that are assigned to
  //     variables).
  int* y;
  y = (int*) malloc( sizeof( int ));

  return y;
  // was in Nov'10: return (int*) malloc( sizeof( int ));
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
