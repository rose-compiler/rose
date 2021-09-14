#include "test-main.h"

const char* description = "Tests subobject access using casts";
const char* expectedout = "{AVBVCXZfaafxx~Z~X~VC~VB~A!}";


struct A
{
  A() : data("a") { printf("A"); }
  virtual ~A() { printf("~A"); }

  const char* data;
};

struct X
{
  X() : data("x") { printf("X"); }
  virtual ~X() { printf("~X"); }

  const char* data;
};

struct B : A
{
  B() : data("b") { printf("B"); }
  ~B() { printf("~B"); }

  const char* data;

};

struct C : A
{
  C() : data("c") { printf("C"); }
  ~C() { printf("~C"); }

  const char* data;
};

struct D : B, C
{
  D() : data("d") { printf("D"); }
  ~D() { printf("~D"); }

  const char* data;
};

struct VB : virtual A
{
  VB() : data("vb") { printf("VB"); }
  ~VB() { printf("~VB"); }

  const char* data;
};

struct VC : virtual A
{
  VC() : data("vc") { printf("VC"); }
  ~VC() { printf("~VC"); }

  const char* data;
};

struct Z : virtual VB, virtual VC, X
{
  Z() : data("z") { printf("Z"); }
  ~Z() { printf("~Z"); }

  const char* data;
};

void fa(const A& obj) { printf("fa%s", obj.data); }
void fx(const X& obj) { printf("fx%s", obj.data); }
void fd(const D& obj) { printf("fd%s", obj.data); }

void run()
{
  Z  z;
  A& a = z;

  fa(a);

  X& x = dynamic_cast<X&>(a);

  fx(x);

  D& d = dynamic_cast<D&>(a); // will fail

  fd(d);
}




