// t0560.cc
// operator= that does not take a reference

struct A {
  void operator=(A) {}
};

struct B : A {};
