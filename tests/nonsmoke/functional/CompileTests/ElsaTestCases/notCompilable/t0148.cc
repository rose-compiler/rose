// t0148.cc
// operator+=, operator-= on pointer types
// based on t0146.cc

struct A {
  operator int* & ();
  //ERROR(2): operator short* & ();     // would create ambiguity
};

struct B {
  operator int* ();
};

struct C {
  void operator+= (int);      // line 15
};

// not a problem to have two conversion operators, if only
// one of them can yield a reference
struct D {
  operator int* & ();
  operator int* ();
};

struct E {
  operator int* volatile & ();
};

enum { ENUMVAL };

void f1()
{
  A a;
  B b;
  C c;
  D d;
  E e;

  a += 3;
  a -= 3;
  __testOverload(c += 3, 15);
  //ERROR(1): b += 3;           // 'b' can't convert to a reference
  d += 3;
  d -= 3;
  e += 3;
  e -= 3;

  // similar to netscape test
  int *mFlags;
  mFlags += ENUMVAL;
  mFlags -= ENUMVAL;
}
