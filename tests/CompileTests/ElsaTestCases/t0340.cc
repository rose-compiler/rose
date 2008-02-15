// t0340.cc
// define a static member of a class using ctor-call syntax

struct A {
  static int x;
  static int y;
  static int z;
};

// no ambiguity
int A::x(5);

enum E { val=5 };

// type-variable ambiguity
int A::y(val);

// error: declare the name as the wrong type
// (currently causes an assertion failure)
//ERROR(1): int A::z(int);

// EOF
