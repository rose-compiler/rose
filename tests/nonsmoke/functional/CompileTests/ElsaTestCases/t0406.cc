// t0406.cc
// tests of 5.2.4: E_fieldAcc with a non-class LHS

// icc gets all of these tests right
// gcc fails to reject tests: 1 4 6 11 12 13 14


typedef int INT;
typedef int const INTC;
typedef int INT2;
typedef float FLOAT;

namespace N {
  typedef int INT3;
  typedef int INT4;
  typedef float FLOAT3;
}

void foo()
{
  int x;
  int const xc = 4;
  int *p = 0;
  INT y;
  INT2 z;

  // unqualified
  x.~INT();
  y.~INT();
  z.~INT();
  p->~INT();

  // 1/2 qualifier
  //ERROR(1): x.::~INT();

  // one qualifier
  x.INT::~INT();
  x.INT::~INT2();
  p->INT2::~INT();
  //ERROR(2): x.INT::~FLOAT();
  //ERROR(3): x.FLOAT::~FLOAT();
  //ERROR(4): x.FLOAT::~INT();

  // one+1/2 qualifier
  x.::INT::~INT();
  x.::INT::~INT2();
  p->::INT2::~INT();
  //ERROR(5): x.::INT::~FLOAT();

  // two qualifiers
  x.N::INT3::~INT3();
  x.N::INT3::~INT4();
  p->N::INT4::~INT3();
  //ERROR(6): x.N::FLOAT3::~INT3();

  // two+1/2 qualifiers
  x.::N::INT3::~INT3();
  x.::N::INT3::~INT4();
  p->::N::INT4::~INT3();
  //ERROR(7): x.N::INT3::~FLOAT3();
  //ERROR(8): x.N::FLOAT3::~FLOAT3();

  // totally nonexistent type names
  //ERROR(9): x.nonexist::~INT();
  //ERROR(10): x.INT::~nonexist();
  //ERROR(15): x.nonexist::~nonexist2();

  // const qualification
  xc.~INTC();
  xc.~INT();                   // ok
  xc.INTC::~INTC();
  xc.INT::~INT();              // ok
  //ERROR(11): xc.INTC::~INT();
  //ERROR(12): xc.INT::~INTC();

  x.~INTC();
  x.~INT();                    // ok
  x.INTC::~INTC();
  x.INT::~INT();               // ok
  //ERROR(13): x.INTC::~INT();
  //ERROR(14): x.INT::~INTC();
}


// EOF
