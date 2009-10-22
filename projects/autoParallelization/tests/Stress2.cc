/**************************************************************************
  Module:  Stress
  Purpose: Utility routines for the solid stress calculations

  Contributed by Jeff Keasler

  Test the handling of math lib function with known side effects
  Liao, 10/22/2009
 ***************************************************************************/
#include <float.h>
#include <math.h>
typedef double real8;

void StressIncrIsoElas(real8 *newSxx,real8 *newSyy,real8 *newSzz,real8 *newTxy,real8 *newTxz,real8 *newTyz,real8 *fun2j,const real8 *dxx,const real8 *dyy,const real8 *dzz,const real8 *dxy,const real8 *dxz,const real8 *dyz,const real8 *shearMod,const int *zoneset,real8 deltaTime,int length)
{
  int i;
  int index;
  real8 twoj;
  real8 twoDelta = (2. * deltaTime);
  for (i = 0; i <= length - 1; i += 1) {
    index = (zoneset[i]);
    newSxx[i] = ((newSxx[i]) + ((twoDelta * (shearMod[index])) * (dxx[index])));
    newSyy[i] = ((newSyy[i]) + ((twoDelta * (shearMod[index])) * (dyy[index])));
    newSzz[i] = ((newSzz[i]) + ((twoDelta * (shearMod[index])) * (dzz[index])));
    newTxy[i] = ((newTxy[i]) + ((twoDelta * (shearMod[index])) * (dxy[index])));
    newTyz[i] = ((newTyz[i]) + ((twoDelta * (shearMod[index])) * (dyz[index])));
    newTxz[i] = ((newTxz[i]) + ((twoDelta * (shearMod[index])) * (dxz[index])));
    twoj = (((((newSxx[i]) * (newSxx[i])) + ((newSyy[i]) * (newSyy[i]))) + ((newSzz[i]) * (newSzz[i]))) + (2. * ((((newTxy[i]) * (newTxy[i])) + ((newTxz[i]) * (newTxz[i]))) + ((newTyz[i]) * (newTyz[i])))));
    fun2j[i] = sqrt((1.5 * twoj));
  }
}
