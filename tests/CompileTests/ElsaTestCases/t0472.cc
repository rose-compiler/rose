// t0472.cc
// do friends inject names?

struct A {
  friend int f(A *a);
  friend int g(int *p);
};

void foo()
{
  // both gcc and icc accept this code, even though it appears to
  // contradict the footnote in 14.6.5 and the rationale at
  // http://groups-beta.google.com/group/comp.std.c++/msg/64407cbd5bb82189
  int *p = 0;
  //ERROR(1): g(p);
  
  // this is legal either by name injection, or by arg-dep lookup
  A *a = 0;
  f(a);
}

// EOF
