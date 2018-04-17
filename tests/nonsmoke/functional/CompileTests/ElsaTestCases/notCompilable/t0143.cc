// t0143.cc
// operator* (unary)

// turn on operator overloading
int dummy();             // line 5
void ddummy() { __testOverload(dummy(), 5); }


struct A {
  operator int* ();
};

typedef int (*Func)();
struct B {
  operator Func ();
};

struct C {
  operator void* ();
};

struct D {
  operator void const* ();
};

struct E {
  operator int();
};


void f1()
{
  A a;
  B b;
  C c;
  D d;
  E e;
  
  *a;
  *b;
  //ERROR(1): *c;     // no built-in derefs void*
  //ERROR(2): *d;     // nor void const* (etc.)
  //ERROR(3): *e;     // nor non-pointers
}


// usual kind of thing
struct F {
  void operator* ();      // line 49
  operator int* ();       // won't interfere
};

// this whacked!
enum G {};
void operator* (G);       // line 55

void f2()
{
  F f;
  G g;
  
  __testOverload(*f, 49);
  __testOverload(*g, 55);
}




