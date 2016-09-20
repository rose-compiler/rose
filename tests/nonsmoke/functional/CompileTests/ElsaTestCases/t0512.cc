// t0512.cc
// const primitive-type variable initialized by IN_ctor
// reported by Jeremy Smithers

const int ConstVar(1234);
int ArrayVar[ConstVar];
