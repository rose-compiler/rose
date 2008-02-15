// t0347.cc
// compare pointers by converting to void*

struct A {
  operator void* ();
};

struct B {
  operator void const * ();
};

void foo()
{
  A a;
  int *p;

  a != p;
  

  // convert to 'void const volatile *' (union the atType cv flags)
  B b;
  int volatile * q;
  
  // this is another one that gcc and icc can't handle, though I
  // believe it to be legal (and Elsa allows it)
  b == q;
}
