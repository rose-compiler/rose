// t0442.cc
// variation of t0438a.cc with all out-of-line defns


template <class T>
struct A;

template <class T1>
struct A {
  template <class U1>
  struct C;
};

template <class T2>
template <class U2>
struct A<T2>::C {
  template <class W2>
  struct F;
};

template <class T3>
template <class U3>
template <class W3>
struct A<T3>::C<U3>::F {
  long f2();
};

template <class T4>
template <class U4>
template <class W4>
long   A<T4>::C<U4>::F<W4>::f2()
{ return 123456789; }


void foo()
{
  A<int>::C<float>::F<char> f;
  f.f2();
}


// EOF
