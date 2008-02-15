// t0438a.cc
// triply-nested templates, a simplification of one aspect
// of t0438.cc


template <class T>
struct A {

  template <class U>
  struct C {

    template <class W>
    struct F {
      long f2();
    };

  };
};

template <class T1>
template <class U1>
template <class W1>
long   A<T1>::C<U1>::F<W1>::f2()
{ return 123456789; }


void foo()
{
  A<int>::C<int>::F<int> f;
  f.f2();
}


// EOF
