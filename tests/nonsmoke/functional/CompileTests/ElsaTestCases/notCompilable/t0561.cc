// t0561.cc
// template parameter of ptr-to-func type

template <int (*f)(int)>
struct A {
  A<f>();

  A<f>(int x)
  {
    f(x);
  }

  int foo(int x)
  {
    return f(x);
  }
};

int f1(int);

A<f1> af1;

int f2(int) { return 1; }

A<f2> af2;
A<(f2)> af2b;
A<((f2))> af3;

A<&f2> af4;
A<(&f2)> af5;
A<((&f2))> af6;
A<((&(f2)))> af7;

// Note that GCC complains about many of the parenthesized versions;
// ICC accepts them, and I see no justification in the standard for
// rejecting.

