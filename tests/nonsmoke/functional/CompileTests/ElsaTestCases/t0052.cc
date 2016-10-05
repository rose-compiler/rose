// cc.in52
// variant of 3.4.5.cc

struct A {
  int a;
  int x;
};
struct B : /*NOT virtual*/ A {};
struct C : B { int q; int x; };
struct D : B { int q; };
struct E : public C, public D {};
struct F : public A{};

void f()
{
  // in these cases, the field being accessed is A::a; the qualifiers
  // cannot be used to specify the base class subobject
  E e;
  //ERROR(1): e.a = 0;                    // ambiguous
  //ERROR(2): e.B::a = 0;                 // ambiguous
  //ERROR(3): e.C::B::a = 0;              // ambiguous

  //ERROR(4): e.q;
  //ERROR(5): e.E::q;
  
  //ERROR(6): e.x;    // ambiguous because A is not inherited virtually

  F f;
  f.A::a = 1;        // OK, A::a is a member of F
}
