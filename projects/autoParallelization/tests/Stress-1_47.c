//#include <float.h>
//#include <math.h>

#define MIN(a, b) ( (a < b) ? a : b)
#define MAX(a, b) ( (a > b) ? a : b)

typedef double real8 ;
   
void StressCheckEpsFail(real8* newSxx, real8* newSyy, real8* newSzz, 
                        real8* newTxy, real8* newTxz, real8* newTyz,
                        real8* eps,    real8 eps_failure_model,
                        const int* zoneset,  int  length)
{
  int i, index;
  for (i = 0 ; i < length ; i++) {
    index = zoneset[i] ;

    if (eps[index] > eps_failure_model){
      newSxx[i] = 0.0 ;
      newSyy[i] = 0.0 ;
      newSzz[i] = 0.0 ;
      newTxy[i] = 0.0 ;
      newTxz[i] = 0.0 ;
      newTyz[i] = 0.0 ;
      eps[index] = eps_failure_model * 1.01;
    }
  }
}

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


void StressStrainHeat(const real8* deltz, real8* deltzh, real8* deltrh,
                      const real8* shearMod, const real8* shearRatio,
                      const real8* shearDer,
                      const real8* newSxx, const real8* newSyy,
                      const real8* newSzz, 
                      const real8* newTxy, const real8* newTxz,
                      const real8* newTyz,
                      const real8* sxx, const real8* syy, const real8* txy, 
                      const real8* txz, const real8* tyz,
                      real8 deltaTime, 
                      const int* zoneset,
                      const real8* vc, const real8* vnewc, int length)
{
   real8 shearr, sheari, avgMod;
   int nz,i;

   deltaTime=deltaTime ; /* Quiet the compiler - unused argument */

   for (i = 0 ; i<length ; i++) {

      nz = zoneset[i] ;
 
      shearr = 0.5*shearRatio[i] ;
      if (shearMod[nz] > 0.) {
         sheari = 0.5/shearMod[nz] ;
         deltrh[nz] = .25 * (vnewc[i] + vc[i]) *
            (
             (newSxx[i]*sheari - sxx[nz]*shearr) * (sxx[nz]  + newSxx[i])  
             + (newSyy[i]*sheari - syy[nz]*shearr) * (syy[nz]  + newSyy[i])  
             + (newSzz[i]*sheari + (syy[nz]+sxx[nz])*shearr) 
             *   (newSzz[i] - sxx[nz] - syy[nz])
             + 2.*(newTxy[i]*sheari - txy[nz]*shearr) 
             * (txy[nz] + newTxy[i])
             + 2.*(newTxz[i]*sheari - txz[nz]*shearr) 
             * (txz[nz] + newTxz[i]) 
             + 2.*(newTyz[i]*sheari - tyz[nz]*shearr) 
             * (tyz[nz] + newTyz[i]) 
             );
      } else {
         deltrh[nz] = -.25 * (vnewc[i] + vc[i]) *
            (
             sxx[nz] * (sxx[nz] + newSxx[i])  
             +  syy[nz] * (syy[nz] + newSyy[i])  
             - (syy[nz]+sxx[nz]) * (newSzz[i] - sxx[nz] - syy[nz])
             + 2.* txy[nz] * (txy[nz] + newTxy[i]) 
             + 2.* txz[nz] * (txz[nz] + newTxz[i]) 
             + 2.* tyz[nz] * (tyz[nz] + newTyz[i])
             ) * shearr ;
      }

      deltzh[nz] = deltz[nz] - deltrh[nz] ;

      avgMod = 0.5 * shearMod[nz];
      if ( shearRatio[i] > 0.0 ) avgMod = avgMod + 0.5 / shearRatio[i];

      if ( avgMod > 0.0 ) 
         deltrh[nz] = shearDer[i] * deltrh[nz] / avgMod ;
      else
         deltrh[nz] = 0.0;
   }
}


