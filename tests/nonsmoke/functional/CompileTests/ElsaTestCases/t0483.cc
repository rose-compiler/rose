// t0483.cc
// virtual inheritance and operator overloading

struct A {
  operator void* ();
};

// Note that if either of the 'virtual' is missing, then the
// code is invalid.  However,
//   - gcc accepts it anyway (it is wrong), and
//   - Elsa gives a misleading error message ("no viable candidate")

struct B : virtual A {};
struct C : virtual A {};

struct D : B, C {};

void foo(D &d)
{            
  d && true;
}


