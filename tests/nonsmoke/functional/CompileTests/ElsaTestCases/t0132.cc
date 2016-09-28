// t0132.cc
// looking more closely at 13.6 para 14

// For every T, where T is a pointer to object type, there exist
// candidate operator functions of the form
//
//   ptrdiff_t operator-(T, T);


struct Int {
  operator int* ();        // line 11
};

struct Float {
  operator float* ();      // line 15
};

// the built-in operators are all non-functions, and 0 is my code for that
enum { BUILTIN=0 };
void jekyl() { __testOverload(jekyl(), 20); }     // turn on operator overloading

void f()                   // line 22
{
  Int i;
  Float f;

  // there is no candidate for this, since int* and float* do not convert
  //ERROR(1): i-f;
}







struct Parent {};
struct Child : Parent {};

struct A {
  operator Parent* ();        // line 41
  operator Child* ();         // line 42
};

struct B {
  operator Child* ();         // line 46
};

void g()
{
  A a;
  B b;

  // candidate operator-(Parent*,Parent*):                              
  //   arg0: A -> Parent* is better than A -> Child* by 13.3.3 para 1
  //         final bullet(SC_IDENTITY)
  //   arg1: B -> Child* which can be converted to Parent* (SC_PTR_CONV)
  //
  // candidate operator-(Child*,Child*):
  //   arg0: A -> Child* is only possibility (SC_IDENTITY)
  //   arg1: B -> Child* (SC_IDENTITY)
  //
  // candidate operator-(Foo*,Foo*):
  //   arg0: no viable conversion candidates
  //   not viable
  //
  // arg0 comparison is indistinguishable, since they use different
  // user-defined conversion operators
  //
  // arg1 comparison favors second candidate, because it's the same
  // user-defined conversion operator, but the final standard
  // conversion is better
  //
  // therefore the winner is
  //   operator-(Child*,Child*)
  a-b;
}




        






struct C {
  operator Parent* ();        // line 90
};

struct D {
  operator Child* ();         // line 94
};

void h()
{
  C c;
  D d;

  // candidate operator-(Parent*,Parent*):
  //   arg0: C -> Parent* (SC_IDENTITY);
  //   arg1: B -> Child* which can be converted to Parent* (SC_PTR_CONV)
  //
  // candidate operator-(Child*,Child*):
  //   arg0: no viable conversion
  //   not viable
  //
  // candidate operator-(Foo*,Foo*):
  //   not viable
  //
  // winner:
  //   candidate operator-(Parent*,Parent*):
  c-d;
}








class Child2 : public Parent {};

struct E {
  operator Child* ();         // line 128
};

struct F {
  operator Child2* ();        // line 132
};

void i()
{
  E e;
  F f;

  // candidate operator-(Parent*,Parent*):
  //   arg0: E -> Child* -> Parent* (SC_PTR_CONV)
  //   arg1: F -> Child2* -> Parent* (SC_PTR_CONV)
  //
  // candidate operator-(Child*,Child*):
  //   arg0: E -> Child* (SC_IDENTITY)
  //   arg1: no viable conversion
  //   not viable
  //
  // candidate operator-(Foo*,Foo*):
  //   not viable
  //
  // winner:
  //   candidate operator-(Parent*,Parent*):
  //
  // NOTE: gcc claims there is no candidate
  e-f;
}






struct G {
  operator int const * ();     // line 165
};

struct H {
  operator int volatile * ();  // line 169
};

void j()
{
  G g;
  H h;

  // candidate operator-(int*,int*):
  //   arg0: not viable
  //   not viable
  //
  // candidate operator-(int const *, int const *):
  //   arg0: SC_IDENTITY
  //   arg1: not viable
  //   not viable
  //
  // candidate operator-(int const volatile *, int const volatile *):
  //   arg0: SC_QUAL_CONV
  //   arg1: SC_QUAL_CONV
  //
  // winner:
  //   candidate operator-(int const volatile *, int const volatile *):
  //
  // again, gcc thinks not
  g-h;
}





struct I {
  operator int const ** ();     // line 202
};

struct J {
  operator int volatile ** ();  // line 206  
};

void k()
{
  I i;
  J j;

  // candidate operator-(int*,int*):
  //   arg0: not viable
  //   not viable
  //
  // candidate operator-(int**,int**):
  //   arg0: not viable
  //   not viable
  //
  // candidate operator-(int const **, int const **):
  //   arg0: SC_IDENTITY
  //   arg1: not viable
  //   not viable
  //
  // candidate operator-(int const volatile **, int const volatile **):
  //   arg0: not viable
  //   not viable
  //
  // candidate operator-(int const volatile * const *, int const volatile * const *):
  //   arg0: SC_QUAL_CONV
  //   arg1: SC_QUAL_CONV
  //
  // winner:
  //   candidate operator-(int const volatile * const *, int const volatile * const *)
  i-j;
}






struct I2 {
  operator int const * const * ();     // line 246
};

struct J2 {
  operator int volatile * const * ();  // line 250
};

void k2()
{
  I2 i;
  J2 j;

  // candidate operator-(int const * const *, int const * const *):
  //   arg0: SC_IDENTITY
  //   arg1: not viable
  //   not viable
  //
  // candidate operator-(int const volatile * const *, int const volatile * const *):
  //   arg0: SC_QUAL_CONV
  //   arg1: SC_QUAL_CONV
  //
  // winner:
  //   candidate operator-(int const volatile * const *, int const volatile * const *):
  //
  // again, gcc thinks not
  i-j;
}



// demonstration that gcc *does* have the right conversion rule
void foo(int const volatile * const *);

void goo()
{
  int const **p = 0;
  foo(p);
}






struct K {
  operator A* ();     // line 291
};

struct L {
  operator B* ();     // line 295
};

void jjj()
{
  K k;
  L l;

  // LUB is void*, but that is not a pointer-to-object type
  //ERROR(2): k-l;
}




typedef int (*funcPtr)();

struct M {
  operator funcPtr ();
};

struct N {
  operator funcPtr ();
};

void mn()
{
  M m;
  N n;

  // LUB is a function pointer type, which is not pointer-to-object
  //ERROR(3): m-n;
}




// example from my notes on the convertibility relation
struct AA {
  operator int* ();
  operator float* ();
};

struct BB {
  operator float* ();
  operator char* ();
};

int ff()
{
  AA a;
  BB b;
  return a-b;     // overloaded operator call
};
