// t0235.cc
// member function delayed instantiation

template <class T1>
struct A {
  int foo(T1 *t)
  { return 2; }

  int someDumbThing()           // will not be instantiated
  { return T1::doesNotExist; }  // so this would-be error is ok


  // used 'T' instead of 'T1'
  //ERROR(1): int bar(T *t)   { return 2; }

  // again, but this time as scope qualifier
  //
  // actually, it's ok if this error isn't diagnosed, according
  // to the standard; but for the moment Elsa does so may as well
  // leave it here
  //
  // 9/21/04: Elsa no longer diagnoses this; I had to weaken its
  // checking for d0102.cc.
  //int anotherDumbThing()
  //{ return T::doesNotExist; }
};

void f()
{
  A<int> a;
  int *x;

  a.foo(x);
}

// EOF
