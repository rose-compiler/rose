// t0552.cc
// enum-valued template parameter

enum E { ZERO, ONE, TWO, THREE };

template <E e>
struct A {};

A<ONE> a1;

//ERROR(1): A<2> a2;
