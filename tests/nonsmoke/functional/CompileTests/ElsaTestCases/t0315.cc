// t0315.cc
// overload resoln with template func and pointer conversion

template <class T>
void f(T const *);

template <class T>
void f(T const *, int);

void foo()
{
  int *i;
  f(i);
}

