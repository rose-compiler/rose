// if the const doesn't get removed, we will have an error

// These don't fail properly because I suppose that elsa isn't
// checking constness.
//  fconst2.cc:9: invalid conversion from `const int*' to `int*'
// OFF ERROR(1): int const * f3(int const x) {}
//  fconst2.cc:13: invalid conversion from `const int*' to `int*'
// OFF ERROR(2): template<class T> T const * f3(T x) {}

// and this works, so it is getting removed
template<class T> T * f1(T const x) {}
// wow, this works also, so it is getting removed at the top level in
// any case
template<class T> T * f2(T x) {}

int main() {
  int const y = 3;
  int *zp1 = f1( (int const) y);
  int *zp2 = f2( (int const) y);
  // OFF ERROR(1): int *zp3 = f3( (int const) y);
  // OFF ERROR(2): int *zp3 = f3( (int const) y);
}
