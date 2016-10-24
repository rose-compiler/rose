// t0514.cc
// use templatized constructor of template class for arg-to-param conversion

template <class S>
struct A {
  template <class T>
  A(T*);
};

void foo(A<int>);
void foo(int);

void bar()
{
  int *p = 0;
  foo(p);
}
