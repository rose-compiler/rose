// t0389.cc
// integer parameter arithmetic ...

template <class T>
struct C {};

template <class T, int n>
struct B {};

template <class T, int n>
struct A {
  typedef B<T, n+1> B_t;
  C<B_t> _a;
};

void f() {
  A<int, 16> a;
}
