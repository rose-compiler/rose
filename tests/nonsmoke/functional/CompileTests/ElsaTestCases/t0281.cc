// t0281.cc
// demonstrate problem with STA_REFERENCE

template <int i>
class A {
};

template <int j>
class B {
  A<j+1> a;
};

B<1> b;
