// t0240.cc
// template function invokes a method on its parameter type
// needed for nsUnicodeToTeXCMRt1.cpp, CallQueryInterface()

template <class T>
void func(T *p)
{
  p->method();
}

// instantiate it

struct A {
  void method();
};

void f()
{
  A *a;
  func(a);
}
