// t0333.cc
// friends that are const member functions

struct A {
  int foo() const;
};

struct B {
  friend int A::foo() const;
};
