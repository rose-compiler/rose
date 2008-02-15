// t0187.cc
// template class with template variable as the base
// (this probably duplicates some other existing test..)

template <class T>
struct C : T {
};

struct B {
  int a;
};
        
template <class T>
struct D {
  C<T> c;
};

int foo()
{
  D<B> d;
  return d.c.a;     // refers to B::a, since C<B> inherits B
}

