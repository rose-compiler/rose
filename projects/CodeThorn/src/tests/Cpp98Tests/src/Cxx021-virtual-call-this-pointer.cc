#include "test-main.h"

const char* description = "Tests construction/destruction sequence with virtual bases";
const char* expectedout = "{VBVXVCVYABfb~B~A~VY~VC~VX~VB}";

struct A
{
  A() : data("a") { printf("A"); }
  ~A() { printf("~A"); }

  const char* data;
};

struct VB
{
  VB() : data("vb") { printf("VB"); }
  ~VB() { printf("~VB"); }

  virtual const char* val() const { return data; }

  const char* data;
};

struct VC
{
  VC() : data("vc") { printf("VC"); }
  ~VC() { printf("~VC"); }

  const char* data;
};

struct VX : virtual VB
{
  VX() : data("vx") { printf("VX"); }
  ~VX() { printf("~VX"); }

  const char* data;
};

struct VY : virtual VB, virtual VC
{
  VY() : data("vy") { printf("VY"); }
  ~VY() { printf("~VY"); }

  const char* data;
};

struct B : A, virtual VX, virtual VY
{
  B() : data("b") { printf("B"); }
  ~B() { printf("~B"); }

  virtual const char* val() const { return data; }

  const char* data;
};

void f(const VB& vb)
{
  printf("f%s", vb.val());
}

void run()
{
  f(B());
}

