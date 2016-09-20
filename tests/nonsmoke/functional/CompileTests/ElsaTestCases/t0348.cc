// t0348.cc
// template argument deduction where a qualification conversion
// is involved

template <class T>
struct L {};

template <class T>
void f(L<T> const *list, int indent);


struct A {};

void foo()
{
  L<A> *list;

  f(list, 3);
}
