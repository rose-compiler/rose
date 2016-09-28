// t0225.cc
// template with conversion operator yielding dependent type
// even simpler version of t0224.cc

template <class T>
struct A {
  operator void*();
  operator T*();
};

void foo()
{
  A<int> a;
  a.operator void* ();
  a.operator int* ();
}

