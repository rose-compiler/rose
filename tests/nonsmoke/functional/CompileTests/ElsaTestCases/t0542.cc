// t0542.cc
// default arg must be taken into account by matching

template <class T>
class A {};

template <class T, class U = A<T> >
class B {};

template <class T>
void f(B<T> &dst, const B<T> &src, int count);

void foo()
{
  B<int> elems;
  
  // this causes us to try to match concrete type
  //   B<int, A<int>>
  // with pattern type
  //   B<T>
  f(elems, elems, 3);
}

