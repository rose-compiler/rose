// t0192.cc
// template function forward declarations

template <class T>
int foo(T *t);

int g()
{
  int *x;
  return foo/*<int>*/(x);     // should be able to deduce args
}

template <class T>
int foo(T *t)
{
  return sizeof(*t) + sizeof(T);
}


