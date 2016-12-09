// t0304.cc
// dereference a conversion-to-const-pointer operator

struct A {
  operator int const * ();     // the 'const' is the issue
};

void foo()
{
  A a;
  *a;
  (a && *a);   // needs polymorphic built-in unary operators to have accurate returns
}
