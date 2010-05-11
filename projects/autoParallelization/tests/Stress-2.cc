#include <float.h>
#include <math.h>

#define MIN(a, b) ( (a < b) ? a : b)
#define MAX(a, b) ( (a > b) ? a : b)

typedef double real8 ;
#if 1
void StressStrainWork(real8* deltz, real8* delts,
                      const real8* newSxx, const real8* newSyy,
                      const real8* newSzz, 
                      const real8* newTxy, const real8* newTxz,
                      const real8* newTyz,
                      const real8* sxx, const real8* syy, const real8* txy, 
                      const real8* txz, const real8* tyz,
                      const real8* dxx, const real8* dyy, const real8* dzz, 
                      const real8* dxy, const real8* dxz, const real8* dyz,
                      real8 deltaTime, 
                      const int* zoneset,
                      const real8* vc, const real8* vnewc,  int length)
{
  int i, index;
  real8 quarterDelta = 0.25 * deltaTime;
  real8 szz;

  for (i = 0 ; i < length ; i++){
    index = zoneset[i];
    szz = - sxx[index] - syy[index] ;
    
    deltz[index] += quarterDelta * (vnewc[i] + vc[i]) *
      (dxx[index] * (sxx[index]  + newSxx[i])  +
       dyy[index] * (syy[index]  + newSyy[i])  +
       dzz[index] * (szz + newSzz[i]) +
       2.*dxy[index] * (txy[index] + newTxy[i]) +
       2.*dxz[index] * (txz[index] + newTxz[i]) +
       2.*dyz[index] * (tyz[index] + newTyz[i]) ) ;

    delts[i] += quarterDelta * (vnewc[i] + vc[i]) *
      (dxx[index] * sxx[index]  +  dyy[index] * syy[index]  +
       dzz[index] * szz           +  2.*dxy[index] * txy[index] +
       2.*dxz[index] * txz[index] +  2.*dyz[index] * tyz[index] ) ;
  }
}

#endif
