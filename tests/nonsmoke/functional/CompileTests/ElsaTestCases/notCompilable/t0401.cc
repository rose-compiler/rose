// t0401.cc
// form pointer-to-member out of overloaded name

struct S {
  int foo();         // line 5
  int foo(int);      // line 6
};

typedef int (S::*ptm1)();
typedef int (S::*ptm2)(int);

void bar()
{
  ptm1 p1 = &S::foo;    // refers to line 5
  ptm2 p2 = &S::foo;    // refers to line 6
}

asm("collectLookupResults foo=5 foo=6");

// EOF
