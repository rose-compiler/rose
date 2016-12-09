// test what happens with const in template function argument
// inference;

// the function template argument inference stuff having to do with
// qualifiers requires so many tests that I put it into another file

// summary of conclusions from experiments with g++:
// at the top level qualifiers are ignored
// below that, they are not unless matched

template<class T> T* f1(T const *x) {}
template<class T> T* f2(T const * const x) {}
template<class T> T* f3(T const * x) {}

// These don't fail properly because I suppose that elsa isn't
// checking constness.
//  fconst1.cc:9: invalid conversion from `const int*' to `int*'
// OFF ERROR(1): int const * f(int const *x) {}
//  fconst1.cc:16: invalid conversion from `const int*' to `int*'
// OFF ERROR(2): template<class T> T* f4(T * const x) {}

int main() {
  int const y = 3;

  int const * yp = &y;
  int const * const yp2 = &y;

  int *zp1 = f1(yp);
  int *zp2 = f2(yp);
  int *zp3 = f3(yp);

  int *zp1b = f1(yp2);
  int *zp2b = f2(yp2);
  int *zp3b = f3(yp2);

// OFF ERROR(1): int *zp1c = f(yp);
// OFF ERROR(2): int *zp1d = f4(yp);
}
