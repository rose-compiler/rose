// t0140.cc
// more operator- stuff

// turn on operator overloading
int dummy();           // line 5
void ddummy() { __testOverload(dummy(), 5); }

//        F     G     .
//        | \ / |     .
//        | / \ |     .
//        H     I     .
//         \   /      .
//          \ /       .
//           J        .
struct F {};
struct G {};
struct H : F, G {};
struct I : F, G {};   
struct J : H, I {};


struct HH {
  operator H* ();
};

struct II {
  operator I* ();
};

int f1()
{
  HH hh;
  II ii;

  // candidate: operator- (F*,F*);
  //   arg0: H*->F* SC_PTR_CONV
  //   arg1: I*->F* SC_PTR_CONV
  //
  // candidate: operator- (G*,G*);
  //   arg0: H*->G* SC_PTR_CONV
  //   arg1: I*->G* SC_PTR_CONV
  //
  // cannot decide
  //ERROR(1): return hh-ii;     // ambiguous
}


// will still be ambiguous
int operator- (void*, void*);

int f2()
{
  HH hh;
  II ii;

  // candidate: operator- (F*,F*);
  //   arg0: H*->F* SC_PTR_CONV
  //   arg1: I*->F* SC_PTR_CONV
  //
  // candidate: operator- (G*,G*);
  //   arg0: H*->G* SC_PTR_CONV
  //   arg1: I*->G* SC_PTR_CONV
  //
  // candidate: operator- (void*, void*);
  //   arg0: H*->void* SC_PTR_CONV
  //   arg1: I*->void* SC_PTR_CONV
  //
  // cannot decide
  //ERROR(2): return hh-ii;     // ambiguous
}


// but this changes everything!
int operator- (HH&, II&);

int f3()
{
  HH hh;
  II ii;

  // candidate: operator- (F*,F*);
  //   arg0: H*->F* SC_PTR_CONV
  //   arg1: I*->F* SC_PTR_CONV
  //
  // candidate: operator- (G*,G*);
  //   arg0: H*->G* SC_PTR_CONV
  //   arg1: I*->G* SC_PTR_CONV
  //
  // candidate: operator- (HH&, II&);
  //   arg0: SC_IDENTITY
  //   arg1: SC_IDENTITY
  //
  // winner:
  //   operator- (HH&, II&);
  return hh-ii;     // not ambiguous!
}





//         A          .
//        / \         .
//       B   C        .
//      /     \       .
//     E       D      .
struct A {};
struct B : A {};
struct C : A {};
struct D : C {};
struct E : B {};

struct BE {
  operator B* ();
  operator E* ();
};

struct CD {
  operator C* ();
  operator D* ();
};

int f4()
{
  BE be;
  CD cd;
  
  // this should cause multiple instantiations of
  //   operator- (A*,A*)
  // though ultimately it's the only viable candidate, and
  // the conversions are unambiguous
  return be-cd;
}



