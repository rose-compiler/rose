// t0330.cc
// lookup bug: failure to diagnose ambiguous lookup

struct Other {
  int foo();
};

struct A {
  int foo();
  typedef Other A3;
};

typedef A A3;

void f()
{
  A a;

  // error: lookup in class != lookup in global
  //ERROR(1): a.A3::foo();
}
