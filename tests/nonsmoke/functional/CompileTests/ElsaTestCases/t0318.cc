// t0318.cc
// static class-valued member

struct S {};

struct A {
  static S x;
};

S A::x;

// EOF
