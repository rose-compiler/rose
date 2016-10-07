// t0353.cc
// even nastier than t0352.cc


template <class T>
struct A {
  int foo(typename T::Inner::template S<int> x);
  int foo(typename T::Inner::template S<float> x);
  int foo(typename T::Inner2::template S<float> x);
};


template <class T>
int A<T>::foo(typename T::Inner::template S<int> x)
{
  return 1;
}


template <class T>
int A<T>::foo(typename T::Inner::template S<float> x)
{
  return 2;
}


template <class T>
int A<T>::foo(typename T::Inner2::template S<float> x)
{
  return 3;
}


class B {
public:
  class Inner {
  public:
    template <class U>
    class S {
    public:
      S();       // silence an icc warning

      U u;
    };
  };

  class Inner2 {
  public:
    template <class U>
    class S {
    public:
      S();       // silence an icc warning

      U u;
    };
  };
};


void f()
{
  A<B> a;
  B::Inner::S<int> i;
  B::Inner::S<float> f;
  B::Inner2::S<float> f2;
  a.foo(i);
  a.foo(f);
  a.foo(f2);
}
