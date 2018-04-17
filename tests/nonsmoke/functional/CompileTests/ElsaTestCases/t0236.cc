// t0236.cc
// member template functions

template <class T1>
struct A {
  template <class V1>
  int foo(T1* x, V1 *w)
  { 
    return sizeof(T1) + sizeof(V1);
  }

  template <class S1>
  int bar(T1 *x, S1 *y);
};

template <class T2>
template <class S2>
int A<T2>::bar(T2 *x, S2 *y)
{
  return sizeof(T2) + sizeof(S2);
}

void f()
{
  A<int> a;
  int *x;
  char *w;
  float *y;

  // instantiate A<int>::foo's inline defn with arg 'char'
  a.foo(x, w);

  // instantiate A<int>::bar's out-of-line defn with arg 'float'
  a.bar(x, y);
}

// EOF
