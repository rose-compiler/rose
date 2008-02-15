// t0138.cc
// testing cppstd 13.6 para 17
  
void dummy();            // line 4

// unfortunately, what's here isn't a very good test because
// nothing ensures the right conversions are chosen, etc; I
// mostly watch the tracing output to verify my expectations..

struct A {
  operator int ();
};

struct B {
  operator float ();
};
  
struct C {
  operator short ();
  operator int ();
};

struct D {
  operator char ();
  operator short ();
};

struct E {
  operator float ();
  operator int ();
};

struct G {
};

void f()
{
  // turn on operator overloading
  __testOverload(dummy(), 4);

  A a;
  B b;
  C c;
  D d;
  E e;
  G g;

  // ok
  a%a;

  // B->float->int
  b%b;

  // ok, because C->short requires a final promotion,
  // whereas C->int does not
  c%a;

  // not ok because can't decide between two promotions
  //ERROR(1): d%a;

  // E->int is better for both
  e%e;

  // no conversions
  //ERROR(2): g%g;

  // round out the menagerie
  a%a;
  a&a;
  a^a;
  a|a;
  a<<a;
  a>>a;
}

