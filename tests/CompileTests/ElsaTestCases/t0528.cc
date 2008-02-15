// t0528.cc
// befriend another class's constructor

// variant of in/gnu/bugs/gb0007.cc, possibly more like what
// that had been minimized from

struct A {
  A(int);
};

struct B {
  friend A::A(int);
};
