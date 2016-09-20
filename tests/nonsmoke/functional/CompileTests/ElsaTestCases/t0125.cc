// t0125.cc
// operator elaboration

struct A {
  int operator * ();
};

A a;

void f()
{
  // un-elaborated
  *a;

  // elaborated
  a.operator *();
}
