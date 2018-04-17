// t0396.cc
// need to not instantiate a default argument

struct A {
  A(int);
};

template <class T>
struct B {
  void f(const T& t = T()) {}
};

void g() {
  B<A> b;
}
