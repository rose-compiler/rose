// t0269.cc
// tricky use of templatized overloaded ctor
// from ostream, basic_string<char>::_M_destroy

template <class T>
struct A {
  A(const A&) {}

  template <class S>
  A(const A<S>&);

  void func(int);
};

void f(A<char> &a)
{
  A<char> obj(a);
  obj.func(3);

  A<char>(a).func(3);
}
