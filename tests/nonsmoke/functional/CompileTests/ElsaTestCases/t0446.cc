// t0446.cc
// template parameter and ?:

template <class T>
int foo(T t)
{
  return t? t->something() : 0;
}

