// t0550.cc
// template arg deduction misses a 'const'

template <class T>
struct A {};

template <class T>
void f(A<T> &x);

void foo(A<int const> &a)
{
  f(a);
}
