// ROSE-2511

#include <stdio.h>      /* printf */
#include <math.h>       /* sqrt */
#include <fenv.h>

#pragma STDC FENV_ACCESS on

int main ()
{
  feclearexcept (FE_ALL_EXCEPT);
  sqrt(-1);
  if (fetestexcept(FE_INVALID)) printf ("sqrt(-1) raises FE_INVALID\n");
  return 0;
}

 
