// CoderInfo-ootm.ii:8:22: error: variable name `D</*ref*/ I0>::P' used as if it were a type

template <int I>
class D {
  union P {};
  static P *L[2];
  static P *M[2];
};

template<int I0>
// the original test case was missing this 'typename' keyword, but
// it is clearly required by 14.6 para 6; gcc-3 requires it, though
// gcc-2 and icc do not
typename
D<I0>::P *D<I0>::L[2] = {0, 0};


// here is the (erroneous) version without 'typename'
//ERROR(1): template<int I0>
//ERROR(1): D<I0>::P *D<I0>::M[2] = {0, 0};


// EOF
