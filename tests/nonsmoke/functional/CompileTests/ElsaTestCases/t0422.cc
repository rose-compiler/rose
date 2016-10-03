// t0422.cc
// duplicate member with dependent type error...

template <int n>
struct A {};

template <class T>
struct B {};

template <class T, int n>
struct C {
  typedef A<n+1> U;
  typedef B<U> V;

  void f(U);
  void f(V);
};
