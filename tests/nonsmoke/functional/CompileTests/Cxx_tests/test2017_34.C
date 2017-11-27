/* fesetround example */
#include <stdio.h>      /* printf */
#include <fenv.h>       /* fesetround, FE_* */
#include <math.h>       /* rint */
#pragma STDC FENV_ACCESS on

int main ()
{
  printf ("rounding -3.8:\n");

  fesetround(FE_DOWNWARD);
  printf ("FE_DOWNWARD: %.1f\n", rint(-3.8));

  fesetround(FE_TONEAREST);
  printf ("FE_TONEAREST: %.1f\n", rint(-3.8));

  fesetround(FE_TOWARDZERO);
  printf ("FE_TOWARDZERO: %.1f\n", rint(-3.8));

  fesetround(FE_UPWARD);
  printf ("FE_UPWARD: %.1f\n", rint(-3.8));
  return 0;
}

