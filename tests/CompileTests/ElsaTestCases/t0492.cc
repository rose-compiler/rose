// t0492.cc
// befriend containing template w/o using template args

template <class T>
struct A {
  struct B {
    friend struct A;
  };
};
