// t0324.cc
// mix of abstract params requiring template argument deduction,
// and concrete params where conversions are allowed

template <class T>
void f(T x, T *y);

template <class T>
void g(int x, T *y);


struct A {
  operator int ();
};

void foo()
{
  A a;
  int *p;

  //ERROR(1): f(a, p);   // does not work b/c first param uses template param
  g(a, p);               // does work b/c first param is concrete
}

// EOF
