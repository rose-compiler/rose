// t0327.cc
// use constructor to satisfy 'class const &' param
// (instantiation problem)

template <class T>
struct A {
  A(T);
};

void f(A<int> const &a);

//A<int> a(4);

void foo()
{
  int i;
  f(i); 
}
