#include <float.h>
#include <math.h>

#define MIN(a, b) ( (a < b) ? a : b)
#define MAX(a, b) ( (a > b) ? a : b)

typedef double real8 ;
   
#if 1

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


#endif
