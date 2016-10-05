// t0541.cc
// virtualness computation gets confused

struct A {
  int f();
};

struct B {
  int f();
};

struct C : A, B {};

struct D : C {
  int f();
};
