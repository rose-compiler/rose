// t0276.cc
// problem with overload resolution when arguments include type vars

     
int foo(int);
int foo(int, int);

template <class S>
void f(S s)
{
  foo(3, s);
}

void g()
{
  int x;
  f(x);
}


// EOF
