// t0572.cc
// Elsa misdiagnosing this in strict mode

template <class T>
struct A {
  void f(char *p)
  {
    g(p);
  }
  
  void g(T *p);
};

void foo()
{
  A<char> a;
  char *p = 0;
  a.f(p);
  
  // confusion partly arises because some specializations are invalid
  A<float> b;
  //ERROR(1): b.f(p);
}
