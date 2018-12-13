
struct B {};

struct A : B {
  using B::operator =;
};

