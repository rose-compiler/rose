// t0277.cc
// another dependent name problem


void bar();
void bar(int *);


template <class T>
void foo(T t)
{
  typedef T *Ptr;    // not a member of a template

  Ptr p;             // will be regarded as non-dependent

  bar(p);            // overload resolution will fail
}

template void foo(int t);
