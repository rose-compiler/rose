// t0352.cc
// nasty example using very dependent types
// (even gcc-3 doesn't get this right, though icc does)


template <class T>
struct A {
  int foo(typename T::template S<int> x);
  int foo(typename T::template S<float> x);
};


template <class T>
int A<T>::foo(typename T::template S<int> x)
{
  return 1;
}


template <class T>
int A<T>::foo(typename T::template S<float> x)
{
  return 2;
}


class B {
public:
  template <class U>
  class S {
  public:
    S();       // silence an icc warning

    U u;
  };
};


void f()
{
  A<B> a;
  B::S<int> i;
  B::S<float> f;
  a.foo(i);
  a.foo(f);
}
