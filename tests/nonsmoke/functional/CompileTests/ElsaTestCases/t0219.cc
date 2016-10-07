// t0219.cc
// more nested template members; more extensive version of t0184.cc

template <class S>
struct A {
  S *s;

  // out-of-line implementation
  template <class T>
  int f(T *t);

  template <class T>
  int g(T *t)
  {
    return 5;
  }
};

template <class S>
template <class T>
int A<S>::f(T *t)
{
  return 6;
}

void foo()
{
  A<int> a;     // instantiate A<int>

  float x;
  a.f(&x);      // instantiate A<int>::f<float>

  a.g(&x);      // instantiate A<int>::g<float>
}
