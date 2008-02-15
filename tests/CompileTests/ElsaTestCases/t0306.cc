// t0306.cc
// ?: with NULL 2nd arg

struct A {};

void f(A *);

void foo(bool b)
{
  A *a;

  f(b? 0 : a);
}
