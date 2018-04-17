// t0301.cc
// convert to pointer for comparison with 0

struct A {
  operator int* ();
};

void f()
{
  A a;
  
  a != 0;
}
