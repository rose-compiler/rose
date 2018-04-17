// t0559.cc
// template class with a method that accepts varargs

template <class T>
struct A {
  void f(int i, ...) { }
};

void foo()
{
  A<int> a;
  a.f(0);
  a.f(0, 3);
  a.f(0, 1, 3);
  a.f(0, 1, 2, 3);
}
