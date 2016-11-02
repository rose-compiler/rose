// t0415.cc
// t0265.cc but with a partial specialization in the mix too

template <class T>
struct A {
  int foo(typename T::type1 x);
  int foo(typename T::type2 x);
};

template <class V>
struct A<V*> {
  int foo(typename V::type1 x);
  int foo(typename V::type2 x);
};


template <class T>
int A<T>::foo(typename T::type1 x)
{
  return 1;
}

template <class V>
int A<V*>::foo(typename V::type1 x)
{
  return 11;
}


template <class S>
int A<S>::foo(typename S::type2 x)
{
  return 2;
}

template <class V2>
int A<V2*>::foo(typename V2::type2 x)
{
  return 12;
}


struct B {
  typedef int type1;
  typedef float type2;
};


void f()
{
  int i = 0;
  float f = 0;

  A<B> a;
  a.foo(i);
  a.foo(f);

  A<B*> a2;
  a2.foo(i);
  a2.foo(f);
}
