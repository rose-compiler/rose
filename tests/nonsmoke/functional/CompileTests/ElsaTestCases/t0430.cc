// t0430.cc
// build a dependent qualified type out of a typedef
 
template <class T>
struct A {
  typedef T *foo_t;
};

struct B {
  typedef int qoo_t;
};

template <class T>
void foo(T t)
{
  typedef A<T> AT;
  typedef typename AT::foo_t bar;

  typedef T TT;
  typedef typename TT::qoo_t barq;
}

void goo()
{
  B b;
  foo(b);
}
