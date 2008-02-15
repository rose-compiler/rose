// t0008.c
// in C it is legal to have a structure member with the same
// name as the class (compare to in/d0087.cc)

struct X {
  int X;     // ok

  int Y;     // ok too
};
