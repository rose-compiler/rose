#include "test-main.h"

const char* description = "Tests subobject pointer shifts with covariant return types";
const char* expectedout = "{A1BA1CDc(gA1BA1CDfac~D~C~A~B~Ag)~D~C~A~B~A}";


struct A
{
  A() : data("a") { printf("A0"); }

  explicit
  A(const char* s) : data(s) { printf("A1"); }

  virtual ~A() { printf("~A"); }

  virtual A* clone() const
  {
    printf("c");
    return new A;
  }

  const char* data;
};

struct B : A
{
  B() : A("ab") { printf("B"); }
  ~B() OVERRIDE { printf("~B"); }

  B* clone() const OVERRIDE
  {
    printf("c");
    return new B;
  }
};

struct C : A
{
  C() : A("ac") { printf("C"); }
  ~C() OVERRIDE { printf("~C"); }

  C* clone() const OVERRIDE
  {
    printf("c");
    return new C;
  }
};

struct D : B, C
{
  D() { printf("D"); }
  ~D() OVERRIDE { printf("~D"); }

  D* clone() const OVERRIDE
  {
    printf("c");
    return new D;
  }
};

void f(const A& obj) { printf("f%s", obj.data); }

void run()
{
  D  d;
  C& c = d;
  // creates D, which must be cast to C (overrides C::clone in this context), and then to A
  // alternative: C* cloned = c.clone();
  A* cloned = c.clone();

  f(*cloned);

  delete cloned;
}





