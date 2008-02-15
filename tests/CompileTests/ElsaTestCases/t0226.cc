// t0226.cc
// explicit call to overloaded conversion operators
// this is a simplified version of t0225.cc

struct A {
  operator void*();
  operator int*();
};

void foo()
{
  A a;
  a.operator void* ();
  a.operator int* ();
  //ERROR(1): a.operator char* ();     // not declared
}

