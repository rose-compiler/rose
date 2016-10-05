// t0314.cc
// another situation of needing to instantiate a template...

struct A {};

template <class T>
struct B : public T {};

void f(A *);

void foo()
{
  B<A> *b;
  f(b);
}


// EOF
