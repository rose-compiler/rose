// t0149.cc
// operator%

struct A {
  operator int& ();
};

struct Av {
  operator int volatile & ();
};

struct B {
  operator float& ();
};

struct Bad {
  operator char ();            // RHS instantiation: int
  operator unsigned char ();    // RHS instantiation: int
};

void f1()
{
  A a;
  Av av;
  B b;
  Bad bad;

  a %= 3;
  av %= 3;
  //ERROR(1): b %= 3;     // can't convert to reference-to-integral
  
  //ERROR(2): a %= bad;
}


