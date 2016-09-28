// t0341.cc
// test where type of unary - must be computed properly.
// then extended for 0L also

class CPPValue {
public:
  CPPValue(long val);
  CPPValue(double val);
};

void foo()
{
  long x;

  // the "0," is to make it clearly an expression; the first case
  // below would otherwise (correctly) be parsed as a declaration, and
  // then diagnosed as erroneous

  0, CPPValue(x);
  0, CPPValue(-x);
  
  0, CPPValue(0L);
}
