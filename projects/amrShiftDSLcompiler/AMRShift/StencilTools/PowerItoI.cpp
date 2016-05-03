#include "PowerItoI.H"
unsigned int Power(unsigned int a_x, unsigned int a_n)
{
  // Basis
  if (a_n == 0)
    return 1;
  else if (a_n == 1)
    return a_x;

  // Induction
  else if (a_n % 2 == 1)
    return a_x * Power(a_x*a_x, a_n/2);
  return Power(a_x*a_x, a_n/2);
};
int ipow(int a, int b)
{
  assert(b >= 0);
  int rtn = 1;
  for (int pow = 0; pow < b; pow++)
  {
     rtn *= a;
  }
  return rtn;
}

double ipow(const double& a, const int& b)
{
  double rtn = 1.0;
  int r=fabs(b);
  for (;r>0; r--)
    rtn*=a;
  return (b>0) ? rtn : 1.0/rtn;
}
