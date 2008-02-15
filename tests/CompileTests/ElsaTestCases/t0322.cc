// t0322.cc
// ok to have a static member that is of incomplete type

struct A;

struct B {
  static A a;
};

// EOF
