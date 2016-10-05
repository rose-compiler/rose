// t0220.cc
// partial specialization of a class template,
// specialized for use with another template.
// needed for iostream

template <class S>
struct A {};

template <class T>
struct B {
  void f();
};

// specialization of B when used with A
template <class U>
struct B< A<U> > {
  void g();
};

void foo()
{
  B<int> b1;          // use the primary
  b1.f();

  B< A<int> > b2;     // use the specialization
  b2.g();
}
