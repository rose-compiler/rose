// t0146.cc
// operator=

// 2005-08-09: It turns out most of this test is invalid because of
// 13.3.1.2p4b2.  So I am replacing uses of operator= with calls to
// 'assign', which behaves the way I had previously implemented
// operator=, thus retaining some of this test's ability to test
// overload resolution.
int &assign(int &, int);
int volatile &assign(int volatile &, int);

// turn on operator overloading
int dummy();                    // line 13
void ddummy() { __testOverload(dummy(), 13); }

struct A {
  operator int& ();
  // Elsa doesn't diagnose this anymore b/c of 'nonstandardAssignmentOperator'
  //-ERROR(2): operator short& ();     // would create ambiguity
};
struct B {
  operator int ();
};

struct C {
  void operator= (int);       // line 26
};

// not a problem to have two conversion operators, if only
// one of them can yield a reference
struct D {
  operator int& ();
  operator int ();
};

struct E {
  operator int volatile & ();
};

enum { ENUMVAL };

void f1()
{
  A a;
  B b;
  C c;
  D d;
  E e;

  assign(a, b);        // was: "a = b"
  __testOverload(c = b, 26);
  //ERROR(1): b.operator int() = b;           // 'b' can't convert to an L&
  assign(d, b);        // was: "d = b"
  assign(e, 3);        // was: "e = 3"
  
  // similar to netscape test; hit all the operators
  int mFlags;
  mFlags = ENUMVAL;
  mFlags *= ENUMVAL;
  mFlags /= ENUMVAL;
  mFlags += ENUMVAL;
  mFlags -= ENUMVAL;
}
