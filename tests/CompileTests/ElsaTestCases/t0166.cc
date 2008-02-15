// t0166.cc
// potential ambiguity in type lookup

struct A {};

struct B : A {};
struct C : A {};

struct D : B, C {
  A *a;       // B::A or C::A?  but same type!
};

