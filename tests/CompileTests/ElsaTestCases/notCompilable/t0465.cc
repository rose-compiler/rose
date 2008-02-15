// t0465.cc
// ambiguous class template parameter

// gcc-3.4.3 accepts both
// icc-8.1 incorrectly rejects the second one


// Currently, Elsa cannot parse this because it does not disambiguate
// declarations properly.  If a declaration is ambiguous, but the
// ambiguity is in the initializer, then both declarations are added
// to the environment, which is bad.
//
// The solution is to reorganize tchecking of declarations so that
// common prefixes are somehow identified and processed only once.


namespace N1 {
  template <int n1, int n2>
  struct A {
    enum { B=2 };
  };
  
  enum { C=5 };

  //         +--------ntparam-----------+
  //                 +------member------+
  //                 +----qual-----+
  //                     +   +arg+
  template < int n = A < 3 , C < 4 > :: B >
  class X {};

  X<> x;         // X<2>
}


namespace N2 {
  enum { A=2 };

  template <int m>
  struct C {
    typedef int B;
  };

  // must add a default for the second param, since the first has one
  template <int n, C<4>::B m = 6>
  class X;

  //                         +--tparam--+
  //         +--ntparam--+   +--member--+
  //                 +def+   +-qual+
  template < int n = A < 3 , C < 4 > :: B >
  class X {};

  X<> x;         // X<2,6>
}


// EOF
