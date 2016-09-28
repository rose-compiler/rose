// t0232.cc
// template class with out-of-line member defn having different
// template parameter lists

template <class T1>
struct A {
  int foo(T1 *t1)        // throw in an inline defn too
  {
    return sizeof(T1);
  }

  int foo2(T1 *t1);
};

template <class T2>
int A<T2>::foo2(T2 *t2)
{
  return sizeof(T2) + sizeof(t2);
}

void f()
{
  A<int> a;
  int *x;

  a.foo2(x);
  a.foo(x);
}

// EOF
