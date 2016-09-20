// t0441.cc
// out-of-line defn of a member template

template <class T>
struct A {
  template <class U>
  struct C;

  template <class V>
  struct D;
};

template <class T>
template <class U>
struct A<T>::C {
  int c1();
};

//ERROR(1): template <class Q2>    // spurious param list
template <class T2>
template <class V2>       //ERRORIFMISSING(2):   // needed param list
struct A<T2>::D {
  int d1();
};

void foo()
{
  A<int>::C<int> c;
  c.c1();

  A<int>::D<int> d;
  d.d1();
}


// EOF
