// t0144.cc
// operator++

// turn on operator overloading
int dummy();                    // line 5
void ddummy() { __testOverload(dummy(), 5); }

struct A {
  operator int& ();
};

struct B {
  void operator++ ();           // line 13
};

struct C {
  operator volatile int& ();
};

struct D {
  operator int ();
};

struct E {
  operator const int& ();
};

struct F {
  void operator++ (int);        // line 29
};

struct G {
  operator bool& ();        
};

struct H {
  operator int*& ();
};

struct I {
  operator int* volatile & ();
};

void f1()
{
  A a;
  B b;
  C c;
  D d;
  E e;
  F f;
  G g;
  H h;
  I i;

  ++a;                          // A::operator int& ()
  __testOverload(++b, 13);      // B::operator ++ ()
  ++c;                          // C::operator volatile int& ()
  //ERROR(1): ++d;              // not an lvalue
  //ERROR(2): ++e;              // can't unify with VQ T&
  ++g;                          // deprecated but legal
  ++h;
  ++i;

  __testOverload(f++, 29);      // F::operator ++ (int)
  a++;
  c++;
  //ERROR(3): d++;              // not an lvalue
  //ERROR(4): e++;              // can't unify with VQ T&
  h++;
  i++;
}
