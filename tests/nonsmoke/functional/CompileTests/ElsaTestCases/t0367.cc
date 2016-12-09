// t0367.cc
// from Altac Edena
// Assertion failed: during type matching, recursion depth exceeded the limit 500, file matchtype.cc line 880

template <class T, class U>
class A {};

template <class T, class U>
A<T, U> f(A<T, U>);

template <class U>
A<int, U> f(A<int, U>);

void g() {
  A<int, double> a;
  f(a);
}
