// t0295.cc
// overloaded operator* and conversion operators returning pointer

struct A {
  int* operator* ();    // use this one
  
  // these are red herrings
  operator int* ();
  operator float* ();
};

// to provoke Elsa's bug I need to obtain the A via a function call,
// instead of just having a variable of type A ...
A getA();

void f()
{
  *getA();
}
