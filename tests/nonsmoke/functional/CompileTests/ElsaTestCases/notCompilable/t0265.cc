// t0265.cc
// does <dependent> match <dependent>?
// needed for ostream, basic_string::replace

template <class T>
struct A {
  int foo(typename T::type1 x);
  int foo(typename T::type2 x);
                                                
  // cannot apply template args to T
  //ERROR(1): int bar(typename T<int>::type3 x);
};


template <class T>
int A<T>::foo(typename T::type1 x)
{
  return 1;
}


template <class T>
int A<T>::foo(typename T::type2 x)
{
  return 2;
}


class B {
  typedef int type1;
  typedef float type2;
};


void f()
{
  A<B> a;
  int i;
  float f;
  a.foo(i);
  a.foo(f);
}
