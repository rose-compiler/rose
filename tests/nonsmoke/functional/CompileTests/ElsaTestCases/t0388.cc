// t0388.cc
// template friend class

struct A {
  template <class T> friend class B;
};
