// t0408.cc
// resolve overloaded name of static functions

struct A {
  static int foo(int);            // line 5
  static int foo(int,int);        // line 6
};

void foo()
{
  A *ap;            // line 11
  A a;              // line 12

  int (*f1)(int) = a.foo;
  int (*f2)(int,int) = ap->foo;
}

asm("collectLookupResults foo=5 a=12 foo=6 ap=11");

