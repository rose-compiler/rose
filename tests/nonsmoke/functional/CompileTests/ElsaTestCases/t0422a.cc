// t0422a.cc
// simplified version of t0422.cc

template <int n>
struct A {};

template <int n>
struct C {
  void f(A<n+1>);
  void f(A<n+2>);
};

template <int n>
void C<n>::f(A<n+1>)
{}

template <int n>
void C<n>::f(A<n+2>)
{}

void foo()
{
  C<1> c;
  A<2> a2;
  A<3> a3;
  c.f(a2);
  c.f(a3);
}
