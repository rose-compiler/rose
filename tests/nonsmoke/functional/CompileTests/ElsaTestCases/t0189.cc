// t0189.cc
// nested class definition that appears outside its containing class

struct A {
  struct B;      // forward decl of nested class
};

struct A::B {
  int x;
};

int foo()
{
  A::B b;
  return b.x;
}

