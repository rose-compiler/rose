// t0503.cc
// DQT in return type, as template arg

template <class T>
struct A {};

template <class T, class U>
struct A2 {};

template <class V>
struct B {
  typedef int I;

  A<I> f();
  A2<I,float> g();
};

template <class V>
A<typename B<V>::I> B<V>::f()
{}

template <class V>
A2<typename B<V>::I, float> B<V>::g()
{}
