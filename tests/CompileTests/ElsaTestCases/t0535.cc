// t0535.cc
// function return type need not be complete at decl of operator()

struct A;

struct B {
  operator A ();
};

