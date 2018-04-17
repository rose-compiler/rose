// t0260.cc
// member func and template member func with similar signatures
// needed for ostream


template <class T>
struct A {
  // member func
  int foo(A *a)
  {
    return sizeof(A);
  }

  //ERROR(1): int foo(A *a)
  //ERROR(1): {
  //ERROR(1):   return sizeof(A);
  //ERROR(1): }

  // template member func
  template <class S>
  int foo(A<S> *a)
  {
    return sizeof(A<S>);
  }

  // member func
  int bar(A *a);
  //ERROR(2): int bar(A *a);

  // template member func
  template <class S>
  int bar(A<S> *a);
};


// EOF
