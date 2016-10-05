// t0258.cc
// template class with member template class
// needed for ostream

template <class S>
struct A {
  // fwd decl of B
  template <class T>
  struct B;

  template <class T>
  struct B {
    int foo(S *s, T *t);

    // inline func defn
    int bar(S *s, T *t)
    {
      return sizeof(S) + sizeof(T);
    }
  };
};

// out-of-line func defn
template <class S2>
template <class T2>
int A<S2>::B<T2>::foo(S2 *s, T2 *t)
{
  return 2 + sizeof(S2) + sizeof(T2);
}

// duplicate defn of 'bar'
//ERROR(1): template <class S2>
//ERROR(1): template <class T2>
//ERROR(1): int A<S2>::B<T2>::bar(S2 *s, T2 *t)
//ERROR(1): {
//ERROR(1):   return 2 + sizeof(S2) + sizeof(T2);
//ERROR(1): }

// duplicate defn of 'foo'
//ERROR(2): template <class S2>
//ERROR(2): template <class T2>
//ERROR(2): int A<S2>::B<T2>::foo(S2 *s, T2 *t)
//ERROR(2): {
//ERROR(2):   return 2 + sizeof(S2) + sizeof(T2);
//ERROR(2): }

// instantiate A's decl
A<int> *aptr;

// instantiate A's defn
A<int> a;

// instantiate B's decl
A<int>::B<float> *bptr;

// instantiate B's defn
A<int>::B<float> b;

void f()
{
  int *ip;
  float *fp;

  b.foo(ip, fp);
  b.bar(ip, fp);

  return 1;
}
