/*  A static variable is initialized to false, but turns true in the false branch of an if.  So the value changes on each call to the function, but the first call is false.
 */

// the use of integer constants is part of the test case
#define FALSE 0
#define TRUE 1

int foo() {
  static int flip = FALSE;
  if(flip) {
    flip = FALSE; // live
  } else { 
    flip = TRUE; // live
  }
}

// unreachable
int bar() {
  return 1;
}

int main() {
  foo();
  foo();
}
