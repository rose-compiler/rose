// t0419.cc
// another from A.E.

struct B {
  template <class U>
  B(const U &);
};

template <class T>
void f(const T & t) {
  B b(t);
}
