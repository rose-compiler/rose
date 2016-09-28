// t0115.cc
// stuff with template functions

template <class T>
void foo(T t)
{
  t;
}

void f()
{
  foo(3);
  foo(3.4);
}
