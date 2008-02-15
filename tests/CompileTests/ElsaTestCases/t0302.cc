// t0302.cc
// convert parenthesized 0 to pointer (doh)

struct A {};

void (*f)(int, A *);

void foo()
{
  (*f)(0, (0) );
}
