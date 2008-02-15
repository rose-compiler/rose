// Example from Jeremiah specific to name qualification.

#include <iostream>

struct X {
  friend int foo(const X&, ...) {return 1;}
};

// Without this line, both calls in main() refer to version 1 of foo().  With
// this added in (for example, to the AST), the first call points here instead,
// and there is no way to qualify it to point to the previous version.  I
// tested this with g++ 4.1.1.
int foo(const X&, int) {return 2;}

int main(int, char**) {
  std::cout << foo(X(), 3) << " " << foo(X(), X()) << std::endl;
  return 0;
}
