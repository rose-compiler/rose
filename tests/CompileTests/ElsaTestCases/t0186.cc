// t0186.cc
// Assertion failed: dt.funcSyntax, file cc_tcheck.cc line 2232

template <class T>
struct A {
  int foo(int (T::*f)());
  int (T::*m)();
};
