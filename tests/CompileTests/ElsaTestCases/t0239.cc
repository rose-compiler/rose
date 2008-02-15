// t0239.cc
// same as t0229, but infer specialization argument

template <class T>
int foo(T *t)
{
  return 2;
}

template <>
int foo(int *t)      // no "<int>", must be inferred
{
  return 3;
}

void f()
{
  int *i;
  float *f;
         
  foo(f);      // use primary
  foo(i);      // use specialization
}
