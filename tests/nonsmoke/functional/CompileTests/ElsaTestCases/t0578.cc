// t0578.cc
// do AbstractEnumerators need to know their value expressions?

// GCC and ICC both reject all six out-of-line member defns

template <int n>
struct A {
  enum { x = n, y = n+1, z /*implicitly y+1*/ };

  int f(int (*a)[n]);
  int f(int (*a)[n+1]);
  int f(int (*a)[n+2]);

  int g(int (*a)[x]);
  int g(int (*a)[y]);
  int g(int (*a)[z]);
};

template <int n>
int A<n>::f(int (*a)[x]) { return 1; }

template <int n>
int A<n>::f(int (*a)[y]) { return 2; }

template <int n>
int A<n>::f(int (*a)[z]) { return 2; }

template <int n>
int A<n>::g(int (*a)[n]) { return 1; }

template <int n>
int A<n>::g(int (*a)[n+1]) { return 2; }

template <int n>
int A<n>::g(int (*a)[n+2]) { return 2; }

