// t0524.cc
// use of operator() must trigger instantiation

template <class T>
struct A {
  void operator()() {}
};

void foo(A<int> &a)
{
  a();
}
