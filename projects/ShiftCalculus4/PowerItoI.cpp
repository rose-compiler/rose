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
