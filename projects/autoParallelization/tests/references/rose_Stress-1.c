//#include <float.h>
//#include <math.h>
#define MIN(a, b) ( (a < b) ? a : b)
#define MAX(a, b) ( (a > b) ? a : b)
#include <omp.h> 
typedef double real8;

void StressCheckEpsFail(real8 *newSxx,real8 *newSyy,real8 *newSzz,real8 *newTxy,real8 *newTxz,real8 *newTyz,real8 *eps,real8 eps_failure_model,const int *zoneset,int length)
{
  int i;
  int index;
  
#pragma omp parallel for private (index,i) firstprivate (eps_failure_model,length)
  for (i = 0; i <= length - 1; i += 1) {
    index = zoneset[i];
    if (eps[zoneset[i]] > eps_failure_model) {
      newSxx[i] = 0.0;
      newSyy[i] = 0.0;
      newSzz[i] = 0.0;
      newTxy[i] = 0.0;
      newTxz[i] = 0.0;
      newTyz[i] = 0.0;
      eps[zoneset[i]] = eps_failure_model * 1.01;
    }
  }
}

void StressStrainWork(real8 *deltz,real8 *delts,const real8 *newSxx,const real8 *newSyy,const real8 *newSzz,const real8 *newTxy,const real8 *newTxz,const real8 *newTyz,const real8 *sxx,const real8 *syy,const real8 *txy,const real8 *txz,const real8 *tyz,const real8 *dxx,const real8 *dyy,const real8 *dzz,const real8 *dxy,const real8 *dxz,const real8 *dyz,real8 deltaTime,const int *zoneset,const real8 *vc,const real8 *vnewc,int length)
{
  int i;
  int index;
  real8 quarterDelta = 0.25 * deltaTime;
  real8 szz;
  
#pragma omp parallel for private (index,szz,i) firstprivate (length,quarterDelta)
  for (i = 0; i <= length - 1; i += 1) {
    index = zoneset[i];
    szz = -sxx[zoneset[i]] - syy[zoneset[i]];
    deltz[zoneset[i]] += quarterDelta * (vnewc[i] + vc[i]) * (dxx[zoneset[i]] * (sxx[zoneset[i]] + newSxx[i]) + dyy[zoneset[i]] * (syy[zoneset[i]] + newSyy[i]) + dzz[zoneset[i]] * (szz + newSzz[i]) + 2. * dxy[zoneset[i]] * (txy[zoneset[i]] + newTxy[i]) + 2. * dxz[zoneset[i]] * (txz[zoneset[i]] + newTxz[i]) + 2. * dyz[zoneset[i]] * (tyz[zoneset[i]] + newTyz[i]));
    delts[i] += quarterDelta * (vnewc[i] + vc[i]) * (dxx[zoneset[i]] * sxx[zoneset[i]] + dyy[zoneset[i]] * syy[zoneset[i]] + dzz[zoneset[i]] * szz + 2. * dxy[zoneset[i]] * txy[zoneset[i]] + 2. * dxz[zoneset[i]] * txz[zoneset[i]] + 2. * dyz[zoneset[i]] * tyz[zoneset[i]]);
  }
}

void StressStrainHeat(const real8 *deltz,real8 *deltzh,real8 *deltrh,const real8 *shearMod,const real8 *shearRatio,const real8 *shearDer,const real8 *newSxx,const real8 *newSyy,const real8 *newSzz,const real8 *newTxy,const real8 *newTxz,const real8 *newTyz,const real8 *sxx,const real8 *syy,const real8 *txy,const real8 *txz,const real8 *tyz,real8 deltaTime,const int *zoneset,const real8 *vc,const real8 *vnewc,int length)
{
  real8 shearr;
  real8 sheari;
  real8 avgMod;
  int nz;
  int i;
  deltaTime = deltaTime;
/* Quiet the compiler - unused argument */
  
#pragma omp parallel for private (shearr,sheari,avgMod,nz,i) firstprivate (length)
  for (i = 0; i <= length - 1; i += 1) {
    nz = zoneset[i];
    shearr = 0.5 * shearRatio[i];
    if (shearMod[zoneset[i]] > 0.) {
      sheari = 0.5 / shearMod[zoneset[i]];
      deltrh[zoneset[i]] = .25 * (vnewc[i] + vc[i]) * ((newSxx[i] * sheari - sxx[zoneset[i]] * shearr) * (sxx[zoneset[i]] + newSxx[i]) + (newSyy[i] * sheari - syy[zoneset[i]] * shearr) * (syy[zoneset[i]] + newSyy[i]) + (newSzz[i] * sheari + (syy[zoneset[i]] + sxx[zoneset[i]]) * shearr) * (newSzz[i] - sxx[zoneset[i]] - syy[zoneset[i]]) + 2. * (newTxy[i] * sheari - txy[zoneset[i]] * shearr) * (txy[zoneset[i]] + newTxy[i]) + 2. * (newTxz[i] * sheari - txz[zoneset[i]] * shearr) * (txz[zoneset[i]] + newTxz[i]) + 2. * (newTyz[i] * sheari - tyz[zoneset[i]] * shearr) * (tyz[zoneset[i]] + newTyz[i]));
    }
     else {
      deltrh[zoneset[i]] = - .25 * (vnewc[i] + vc[i]) * (sxx[zoneset[i]] * (sxx[zoneset[i]] + newSxx[i]) + syy[zoneset[i]] * (syy[zoneset[i]] + newSyy[i]) - (syy[zoneset[i]] + sxx[zoneset[i]]) * (newSzz[i] - sxx[zoneset[i]] - syy[zoneset[i]]) + 2. * txy[zoneset[i]] * (txy[zoneset[i]] + newTxy[i]) + 2. * txz[zoneset[i]] * (txz[zoneset[i]] + newTxz[i]) + 2. * tyz[zoneset[i]] * (tyz[zoneset[i]] + newTyz[i])) * shearr;
    }
    deltzh[zoneset[i]] = deltz[zoneset[i]] - deltrh[zoneset[i]];
    avgMod = 0.5 * shearMod[zoneset[i]];
    if (shearRatio[i] > 0.0) 
      avgMod = avgMod + 0.5 / shearRatio[i];
    if (avgMod > 0.0) 
      deltrh[zoneset[i]] = shearDer[i] * deltrh[zoneset[i]] / avgMod;
     else 
      deltrh[zoneset[i]] = 0.0;
  }
}
