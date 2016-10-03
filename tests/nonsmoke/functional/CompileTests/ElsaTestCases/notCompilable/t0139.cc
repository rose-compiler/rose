// t0139.cc
// overloaded operator && and ||

// turn on operator overloading
void dummy();           // line 5
void f1() { __testOverload(dummy(), 5); }

struct A {
  operator bool ();
};

void f2()
{
  A a;
  __testOverload(a&&a, 0);    // built-in &&
}

struct B {
  void operator&& (B const &b) const;    // line 19
};

void f3()
{
  B b;
  __testOverload(b&&b, 19);

  // no conversion from B to bool, nor from A to 'B const &'
  //ERROR(1): b&&a;
}

bool operator&& (bool x, bool y);       // line 31

void f4()
{
  A a;                                 
  
  // now this uses the user's operator function
  __testOverload(a&&a, 31);
}


