// t0534.cc
// template argument deduction with partially-specified arguments

// This tests 14.8.1p4, which says that if a parameter is
// fully-specified (concrete) by the explicitly-provided
// template arguments, then you can use conversions to get
// the argument to match the parameter.  Otherwise, the
// match must be nearly exact.

template <class T>
void f(const T &a, const T &b);

template <class T, class U>
void g(const T &a, const T &b, U u);

template <class T>
void h(T, T);

template <class T>
void j(T, unsigned);

void foo()
{
  unsigned u;
  int i;

  //ERROR(1): f(u, i);
  f<unsigned>(u, i);

  g<unsigned>(u, i, 3);

  //ERROR(2): h(u, i);
  h<unsigned>(u, i);

  j<unsigned>(u, i);

  j(u, i);
}
