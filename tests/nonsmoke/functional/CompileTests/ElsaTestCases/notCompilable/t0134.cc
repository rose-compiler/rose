// t0134.cc
// test 'computeLUB'

// codes from overload.h
enum {
  BAD=0,
  OK=1,
  AMBIG=2
};

//         A          .
//        / \         .
//       B   C        .
//        \ / \       .
//         E   D      .
struct A {};
struct B : A {};
struct C : A {};
struct E : B, C {};
struct D : C {};

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

enum Enum1 {};
enum Enum2 {};

void f()
{
  // straightforward cv
  __computeLUB((int*)0, (int*)0, (int*)0, OK);
  __computeLUB((int*)0, (float*)0, 0, BAD);
  __computeLUB((int*)0, (int const*)0, (int const*)0, OK);
  __computeLUB((int volatile*)0, (int const*)0, (int volatile const*)0, OK);

  // tricky cv
  __computeLUB((int volatile **)0,
               (int const **)0,
               (int volatile const * const *)0, OK);

  __computeLUB((int volatile * const *)0,
               (int const * const *)0,
               (int volatile const * const *)0, OK);

  __computeLUB((int volatile **)0,
               (int const * volatile *)0,
               (int volatile const * const volatile *)0, OK);

  // put some pointer-to-member into the cv mix
  __computeLUB((int A::*)0, (int A::*)0, (int A::*)0, OK);
  __computeLUB((int const A::*)0, (int A::*)0, (int const A::*)0, OK);
  __computeLUB((int A::**)0, (int A::**)0, (int A::**)0, OK);

  __computeLUB((int const A::**)0,
               (int volatile A::**)0,
               (int const volatile A::* const *)0, OK);

  __computeLUB((int const A::* B::*)0,
               (int volatile A::* B::*)0,
               (int const volatile A::* const B::*)0, OK);

  __computeLUB((int const C::* B::*)0,
               (int volatile A::* B::*)0,
               0, BAD);
               
  // this should actually yield 'int E::*', but I don't implement
  // it properly
  __computeLUB((int B::*)0, (int C::*)0, 0, BAD);

  // basic inheritance stuff
  __computeLUB((A*)0, (B*)0, (A*)0, OK);
  __computeLUB((C*)0, (B*)0, (A*)0, OK);
  __computeLUB((E*)0, (D*)0, (C*)0, OK);
  __computeLUB((E*)0, (A*)0, (A*)0, OK);
  __computeLUB((B*)0, (D*)0, (A*)0, OK);

  // same, but play some games with cv qualification
  __computeLUB((A const *)0, (B*)0, (A const *)0, OK);
  __computeLUB((C*)0, (B volatile *)0, (A volatile *)0, OK);
  __computeLUB((E const *)0, (D const *)0, (C const *)0, OK);
  __computeLUB((B const *)0, (D volatile *)0, (A const volatile *)0, OK);

  // with ambiguity
  __computeLUB((H*)0, (G*)0, (G*)0, OK);
  __computeLUB((H*)0, (I*)0, 0, AMBIG);
  __computeLUB((F*)0, (J*)0, (F*)0, OK);
  __computeLUB((J*)0, (I*)0, (I*)0, OK);

  // across disconnected hierarchies
  __computeLUB((F*)0, (A*)0, (void*)0, OK);
  __computeLUB((H const *)0, (D volatile *)0, (void const volatile *)0, OK);
  
  // with enums
  __computeLUB((Enum1)0, (Enum1)0, (Enum1)0, OK);
  __computeLUB((Enum2)0, (Enum1)0, 0, BAD);
  __computeLUB((void*)0, (Enum1)0, 0, BAD);
  
  // according to my spec for computeLUB, it's only relevant that
  // this does *not* yield Enum1; if at some point it changes to
  // yield int, that's ok (just change the test to match)
  __computeLUB((int)0, (Enum1)0, 0, BAD);
}
