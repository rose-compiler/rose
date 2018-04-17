// t0402.cc
// resolve overloaded function name in assignment or initialization


int foo();         // line 5
int foo(int);      // line 6

typedef int (*pf1)();
typedef int (*pf2)(int);

// assignment expressions
void baz()
{
  pf1 p1;
  p1 = foo;    // refers to line 5

  pf2 p2;
  p2 = foo;    // refers to line 6
}

// object initialization
void bar()
{
  pf1 p1 = foo;    // refers to line 5
  pf2 p2 = foo;    // refers to line 6
}

asm("collectLookupResults p1=14 foo=5 p2=17 foo=6 foo=5 foo=6");

// EOF
