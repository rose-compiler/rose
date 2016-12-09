// t0508.cc
// template accepts reference, pointer and pointer-to-member

template <int &IR>
int f()
{
  &IR;       // legal
  return IR + 10;
}


template <int *IP>
int g()
{
  return *IP + 20;
}


struct S {
  int m;
};

template <int S::*PTM>
int h(S *s)
{
  return s->*PTM;
}


int x;

void foo()
{
  f<x>();
  g<&x>();
  
  S *s = 0;
  h<&S::m>(s);
}
