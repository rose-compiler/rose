// t0275.cc
// template function definition with an error


typedef int barf;

namespace M {
  template <class T>
  class A {
    int foo(int);
  };
                       
  //ERROR(1): int barf;      // hides type name above

  template <class T>
  int A<T>::foo(barf)
  {
    return 2;
  }
}


namespace N {
  //ERROR(2): int barf;      // hides type name above

  template <class T>
  class B {
    int f(int);
  };
}

template <class T>
int N::B<T>::f(barf)
{
  return 3;
}

