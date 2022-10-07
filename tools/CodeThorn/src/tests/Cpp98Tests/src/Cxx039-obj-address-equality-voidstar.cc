#include "test-main.h"

const char* description = "Compares various subobjects of a class";
const char* expectedout = "{ABACD==1==1==1==1==1==1==1==1==1==1~D~C~A~B~A}";

struct A
{
  A() : data("a") { printf("A"); }

  virtual ~A() { printf("~A"); }

  const char* data;
};

struct B : A
{
  B() : data("b") { printf("B"); }
  ~B() OVERRIDE { printf("~B"); }

  const char* data;
};

struct C : A
{
  C() : data("c") { printf("C"); }
  ~C() OVERRIDE { printf("~C"); }

  const char* data;
};

struct D : B, C
{
  D() : data("d") { printf("D"); }
  ~D() OVERRIDE { printf("~D"); }

  const char* data;
};

void equals(const A* lhs, const A* rhs)
{
  char c = dynamic_cast<const void*>(lhs) == dynamic_cast<const void*>(rhs) ? '1':'0';

  printf("==%c", c);
}

void run()
{
  D  d;
  B& b  = d;
  A& ab = b;
  C& c  = d;
  A& ac = c;

  equals(&b, &b);
  equals(&b, &ab);
  equals(&b, &c);
  equals(&b, &ac);

  equals(&ab, &ab);
  equals(&ab, &c);
  equals(&ab, &ac);

  equals(&c, &c);
  equals(&c, &ac);

  equals(&ac, &ac);
}

