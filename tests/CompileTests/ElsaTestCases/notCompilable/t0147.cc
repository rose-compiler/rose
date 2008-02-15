// t0147.cc
// inherit a conversion operator

struct A {
  operator int& ();
};

struct B : A {
  operator int ();
};

int &assign(int &, int);

void f1()
{
  B b;

  // needs B to inherit A::operator int& ()
  assign(b, 3);
}
