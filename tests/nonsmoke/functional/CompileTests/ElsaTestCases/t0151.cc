// t0151.cc
// operator->*
       
//         A      D   .
//        / \         .
//       B   C        .
//      /             .
//     E              .
struct A {};
struct B : A {};
struct C : A {};
struct D {};
struct E : B {};

struct Ap {
  operator A* ();
};

struct Apm {
  operator int A::* ();
};

struct Bp {
  operator B* ();
};

struct Bpm {
  operator int B::* ();
};

struct Cp {
  operator C* ();
};

struct CDpm {
  operator int C::* ();
  operator int D::* ();
};

struct ADp {
  operator A* ();
  operator D* ();
};

struct AEp {
  operator A* ();
  operator E* ();
};

struct BEp {
  operator B* ();
  operator E* ();
};

struct BCp {
  operator B* ();
  operator C* ();
};

void f1()
{
  Ap ap;
  Apm apm;
  Bp bp;
  Bpm bpm;
  Cp cp;
  CDpm cdpm;
  ADp adp;
  AEp aep;
  BEp bep;
  BCp bcp;

  ap->*apm;

  bp->*bpm;

  bp->*apm;

  cp->*apm;

  // both possible conversions for 'cdpm' yield classes that
  // are not superclasses of B
  //ERROR(1): bp->*cdpm;

  // ADp::operator int D::* () isn't in a viable pair
  adp->*apm;

  // neither conversion for 'adp' yields a viable pair
  //ERROR(2): adp->*bpm;

  // these are all ambiguous, because all of the viable pairs
  // are perfect conversions
  //ERROR(3): aep->*apm;
  //ERROR(4): bep->*apm;
  //ERROR(5): bcp->*apm;

  // one pair goes wrong way in inheritance hierarchy, the other is
  // not related all
  //ERROR(6): ap->*cdpm;

  // repeat all the good ones, to exercise my instantiation cache
  ap->*apm;
  bp->*bpm;
  bp->*apm;
  cp->*apm;
  adp->*apm;
}
