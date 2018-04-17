// t0259.cc
// tricky "duplicate" definitions
// needed for ostream


template <class T>
int foo(T *x, T *t)
{
  return 1;
}

template <class T>
int foo(T *x, T const *t)
{
  return 2;
}


template <class T>
int bar(T const *t)
{
  return 1;
}

template <class T>
int bar(T *t)
{
  return 2;
}
