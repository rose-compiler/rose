// t0399.cc
// a couple tricky(?!) qualified lookups

struct A {
  static int i;
};

struct B : A {};

struct C : B {};

void foo()
{
  int x;
  C *c;

  x = c->B::A::i;
  
  x = B::A::i;
}
