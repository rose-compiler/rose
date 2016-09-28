// t0494.cc
// overloaded functions differing only in constrained type equalities

// for the current bug to manifest, the less-general version
// must come first in the file
template <class W>
void f(W x, W y) {}

template <class T, class U>
void f(T x, U y) {}

void foo()
{
  f(1, 2);
  f(1, 2.3);
}
