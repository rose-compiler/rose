// t0335.cc
// delete an incomplete type (5.3.5 para 5)

class A;

A *p;

void foo()
{
  delete p;
}          

// this program is valid (has defined behavior) only if this
// definition appears (e.g. in another translation unit) with
// trivial destructor
//class A {};
