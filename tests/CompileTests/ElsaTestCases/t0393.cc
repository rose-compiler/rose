// t0393.cc
// another template test from A.E.: enum from instantiated template
// as a template arg

template <int n>
struct C {};

template <int n>
struct B {
  enum {
    N = n
  };
};

struct A {
  typedef B<16> B_t;
  C<B_t::N> _c;
};
