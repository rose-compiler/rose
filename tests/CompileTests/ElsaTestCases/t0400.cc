// t0400.cc
// instantiate template function where function accepts template
// class instance which is *not* instantiated, such that when
// we tcheck the function instantiation's declaration we have
// to instantiate the class (subtle scope insertion/deletion
// ordering bug)

template <class T>
struct S {};

template <class T>
S<T> foo(S<T> *s)
{
  S<T> x;
  return x;
}

void bar()
{
  S<int> *s = 0;     // does not instantiate S<int>
  foo(s);
}

