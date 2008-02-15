// t0380.cc
// testing an aspect of non-dependent lookup

int f(char);             // line 4

template <class T>
int foo(T *t)            // line 7
{
  return f(4);           // should get 'f' on line 4
}

int f(int);              // line 12

void bar()
{
  int *y = 0;            // line 16
  foo(y);
}

asm("collectLookupResults f=4 foo=7 y=16");
