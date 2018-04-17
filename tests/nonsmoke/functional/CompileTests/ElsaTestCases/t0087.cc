// t0087.cc
// experimenting with ptr-to-member

class A {
public:
  int x;
};
class B {};
                            
// just let me inherit from A in a way that all subobjects
// can still be unambiguously named; g++ has a warning for this,
// "warning: direct base `A' inaccessible in `D' due to ambiguity"
class A_prime : public A {};

class C : public A {};
class D : public C, public A_prime {};

class E : virtual public A {};
class F : public E, virtual public A {};
class G : public E, public A_prime {};

void foo()
{
  int A::*p = &A::x;

  // experiments with .*
  A a;
  B b;
  C c;
  D d;
  E e;
  F f;
  G g;

  a.*p = 7;                 // ok, obviously
  //ERROR(1): b.*p = 7;     // not derived
  c.*p = 7;                 // ok, A is a base of C
  //ERROR(2): d.*p = 7;     // ambiguous derivation
  e.*p = 7;                 // ok, A is a virtual base of E
  f.*p = 7;                 // ok, A is a virtual base twice (E and F)
  //ERROR(3): g.*p = 7;     // ambiguous derivation

  // same things with ->*
  A *ap;
  B *bp;
  C *cp;
  D *dp;
  E *ep;
  F *fp;
  G *gp;

  ap->*p = 7;               // ok, obviously
  //ERROR(4): bp->*p = 7;   // not derived
  cp->*p = 7;               // ok, A is a base of C
  //ERROR(5): dp->*p = 7;   // ambiguous derivation
  ep->*p = 7;               // ok, A is a virtual base of E
  fp->*p = 7;               // ok, A is a virtual base twice (E and F)
  //ERROR(6): gp->*p = 7;   // ambiguous derivation
}


