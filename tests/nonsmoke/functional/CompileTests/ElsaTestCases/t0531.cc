// t0531.cc
// confusion between operator*&() and operator=()

struct A {
  A (const char *);
  operator char *&();
};

void foo(A &s)
{
  s = "0";
}



struct B {
  operator int& ();
};

void foo(B &b)
{
  // this would violate 13.3.1.2p4b2
  //ERROR(1): a = 2;
}

