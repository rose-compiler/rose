// t0421.cc
// n+1 as template argument..

template <int n>
struct A {};

template <class T>
struct B {};

template <int n>
struct C {
  typedef B<A<n+1> > V;
};
