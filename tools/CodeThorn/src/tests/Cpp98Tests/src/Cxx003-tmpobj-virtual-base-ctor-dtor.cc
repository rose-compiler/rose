#include "test-main.h"

const char* description = "Tests construction/destruction sequence with virtual bases";
const char* expectedout = "{VDVB0VXVCVYABCfvb~C~B~A~VY~VC~VX~VB~VD}";

struct A
{
  A()  { printf("A"); }
  ~A() { printf("~A"); }
};

struct VB
{
  VB() : data("vb")           { printf("VB0"); }
  VB(const char* s) : data(s) { printf("VB1"); }
  ~VB()                       { printf("~VB"); }

  const char* data;
};

struct VC
{
  VC()  { printf("VC"); }
  ~VC() { printf("~VC"); }
};

struct VD
{
  VD()  { printf("VD"); }
  ~VD() { printf("~VD"); }
};

struct VX : virtual VB
{
  VX() : VB("vbx") { printf("VX"); }
  ~VX() { printf("~VX"); }
};

struct VY : virtual VB, virtual VC
{
  VY() : VB("vby") { printf("VY"); }
  ~VY() { printf("~VY"); }
};

struct B : virtual VD
{
  B()  { printf("B"); }
  ~B() { printf("~B"); }
};

struct C : A, B, virtual VX, virtual VY
{
  C()  { printf("C"); }
  ~C() { printf("~C"); }
};

void f(const VB& obj)
{
  printf("f%s", obj.data);
}

void run()
{
  f(C());
}

