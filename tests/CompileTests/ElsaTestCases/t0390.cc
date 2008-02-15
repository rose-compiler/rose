// t0390.cc
// ctor member init via a typedef

struct B {};

struct A : public B {
  typedef B S;
  A() : S() {}
};
