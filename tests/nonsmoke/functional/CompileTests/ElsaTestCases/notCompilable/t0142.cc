// t0142.cc
// operator+

// turn on operator overloading
int dummy();             // line 5
void ddummy() { __testOverload(dummy(), 5); }

struct A {
  operator void* ();
};
          
typedef int (*FuncPtr)();
struct B {
  operator FuncPtr ();
};

struct C {
  operator C* ();
};

struct D {
  operator int ();
};

struct E {
  operator C* ();
  operator int ();
};

void f1()
{
  A a;
  B b;
  C c;
  D d;
  E e;

  +a;
  +b;
  +c;
  +d;     
  
  // ambiguous
  //ERROR(1): +e;
}

