// t0229.cc
// function template with complete specialization

template <class T>
int foo(T *t)
{
  return 2;
}

template <>
int foo<int>(int *t)
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
