// t0416.cc
// dependent type matching that does not involve qualifiers
// but nevertheless isomorphic type matching is insufficient

// interesting: can only be instantiated if S != T
template <class S, class T>
struct A {
  int foo(S *s, T *t);           // line 8
  int foo(T *t, S *s);           // line 9
};

template <class S1, class T1>
int A<S1,T1>::foo(S1 *s, T1 *t)
{
  return 1;
}

template <class S2, class T2>
int A<S2,T2>::foo(T2 *t, S2 *s)
{
  return 2;
}

int main()
{
  A<int,float> a;
  int *i = 0;
  float *f = 0;

  // same type will not work
  //ERROR(1): A<int,int> a2;

  int x = __testOverload(a.foo(i,f), 8);            // foo on line 8
  int y = __testOverload(a.foo(f,i), 9);            // foo on line 9

  if (x==1 && y==2) {
    return 0;
  }
  else {
    return 4;
  }
}
