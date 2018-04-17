// t0230.cc
// function template overloadings that differ only in constness under pointer
// needed for ostream.h, __copy_aux2

// overloading 0: most general
template <class S>
int foo(S x)                      // line 7
{
  return 1;
}

// overloading 1: pointer to non-const
template <class T>
int foo(T *x)                     // line 14
{
  return 2;
}

// overloading 2: pointer to const
template <class T>
int foo(T const *x)               // line 21
{
  return 3;
}

// use them
void f()
{
  int x;
  int *y;
  int const *z;

  __testOverload(foo(x), 7);        // use 0
  __testOverload(foo(y), 14);       // use 1
  __testOverload(foo(z), 21);       // use 2
}
