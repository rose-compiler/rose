// t0209.cc
// type equality with and w/o default args supplied

template <class T = int>
class C {};

int f()
{    
  // these should have the same type
  C<> *p;
  C<int> *q;

  // hence conversion between them should be identity
  // (casts needed to avoid them being considered references...)
  __getStandardConversion((C<>*)p, (C<int>*)q, 0 /*SC_IDENTITY*/);

  return 8;
}
