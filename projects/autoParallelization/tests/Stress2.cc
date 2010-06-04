/**************************************************************************
  Module:  Stress
  Purpose: Utility routines for the solid stress calculations
 ***************************************************************************/

#include <float.h>
#include <math.h>

#define MIN(a, b) ( (a < b) ? a : b)
#define MAX(a, b) ( (a > b) ? a : b)

typedef double real8 ;

#if 1
/************************************************************************
 * Function  : StressJaumRot
 * Purpose   : Increment the stress by an amount corresponding to
 *             the rotation terms in the Jaumann stress rate.
 ************************************************************************/

void StressJaumRot(real8* newSxx, real8* newSyy, real8* newSzz,
                   real8* newTxy, real8* newTxz, real8* newTyz, 
                   const real8* sxx, const real8* syy, const real8* txy,    
                   const real8* txz, const real8* tyz, const real8* wxx,    
                   const real8* wyy, const real8* wzz,
                   const int* zoneset,  real8  deltaTime, int length)
{
  int index, i;
  for (i = 0 ; i < length ; i++){
    index = zoneset[i] ;
 
    newSxx[i]  = sxx[index] + deltaTime *
      (-2. * txy[index] * wzz[index] + 2. * txz[index] * wyy[index] );

    newSyy[i]  = syy[index] + deltaTime *
      ( 2. * txy[index] * wzz[index] - 2. * tyz[index] * wxx[index] );
 
    newSzz[i]  = - sxx[index] - syy[index] + deltaTime *
      (-2. * txz[index] * wyy[index] + 2. * tyz[index] * wxx[index] ) ;
 
 
    newTxy[i] = txy[index]  + deltaTime *
      ( wzz[index] * ( sxx[index] - syy[index] )  +
        wyy[index] * tyz[index] - wxx[index] * txz[index] ) ;
 
    newTyz[i] = tyz[index]  + deltaTime *
      ( wxx[index] * ( 2. * syy[index] + sxx[index] )  +
        wzz[index] * txz[index] - wyy[index] * txy[index] ) ;
 
    newTxz[i] = txz[index]  + deltaTime *
      ( wyy[index] * ( -syy[index] - 2. * sxx[index] )  +
        wxx[index] * txy[index] - wzz[index] * tyz[index] ) ; 

  }
}


/************************************************************************
 * Function  : StressIncrIsoElas
 * Purpose   : Increment the deviatoric stresses assuming
 *             isotropic elastic behavior.  Also update the
 *             corresponding value of  sqrt(3/2) * J2.
 ************************************************************************/
void StressIncrIsoElas(real8* newSxx, real8* newSyy, real8* newSzz, 
                       real8* newTxy, real8* newTxz, real8* newTyz,
                       real8* fun2j,
                       const real8* dxx, const real8* dyy, const real8* dzz, 
                       const real8* dxy, const real8* dxz, const real8* dyz,
                       const real8* shearMod, 
                       const int* zoneset, real8 deltaTime, int length)
{
  int i, index;
  real8  twoj ;
  real8  twoDelta = 2.0 * deltaTime;

  for (i = 0 ; i < length ; i++){

    index = zoneset[i] ;
 
    newSxx[i]  = newSxx[i] + twoDelta * shearMod[index] * dxx[index] ;
 
    newSyy[i]  = newSyy[i] + twoDelta * shearMod[index] * dyy[index] ;

    newSzz[i]  = newSzz[i] + twoDelta * shearMod[index] * dzz[index] ;
 
    newTxy[i]  = newTxy[i] + twoDelta * shearMod[index] * dxy[index] ;
 
    newTyz[i]  = newTyz[i] + twoDelta * shearMod[index] * dyz[index] ;
 
    newTxz[i]  = newTxz[i] + twoDelta * shearMod[index] * dxz[index] ;
 
    twoj = newSxx[i] * newSxx[i] + newSyy[i] * newSyy[i] + 
      newSzz[i] * newSzz[i] + 
      2.0 * (newTxy[i] * newTxy[i] +  newTxz[i] * newTxz[i]
      + newTyz[i] * newTyz[i]) ;
 
    fun2j[i]= sqrt(1.5 * twoj) ;
  }
}


/************************************************************************
 * Function  : StressScale
 * Purpose   : 
 ************************************************************************/
void StressScale(real8* Sxx, real8* Syy, real8* Szz, 
                        real8* Txy, real8* Txz, real8* Tyz,
                        real8* fun2j,
                        const real8* shearMod, const real8* shearRatio,
                        const int* zoneset, int length, int Softening)
{
  int i;
  real8  twoj ;

  if ( Softening == 0 ) {
     /* Do Nothing */
  } else {
     for (i = 0 ; i < length ; i++){
        
        int index = zoneset[i] ;
        real8 rat = shearRatio[i] * shearMod[index] ;
        
        Sxx[i]  = Sxx[i] * rat ;
        
        Syy[i]  = Syy[i] * rat ; 
        
        Szz[i]  = Szz[i] * rat ; 
        
        Txy[i]  = Txy[i] * rat ; 
        
        Tyz[i]  = Tyz[i] * rat ; 
        
        Txz[i]  = Txz[i] * rat ; 
        
        twoj = Sxx[i]*Sxx[i] + Syy[i]*Syy[i] + Szz[i]*Szz[i] + 
           2.0 * (Txy[i]*Txy[i] +  Txz[i]*Txz[i] + Tyz[i]*Tyz[i]) ;
        
        fun2j[i]= sqrt(1.5 * twoj) ;
     }
  }
  
}


/************************************************************************
 * Function  : StressCheckYieldJ2
 * Purpose   : Correct stress with radial return if current (trial) stress
 *             lies outside the current (mises) yield surface.
 ************************************************************************/
void StressCheckYieldJ2(real8* newSxx, real8* newSyy, real8* newSzz, 
                        real8* newTxy, real8* newTxz, real8* newTyz,
                        const real8* yield,  const real8* fun2j,
                        const int* zoneset, int length)
{
  int i, index;
  real8  scale;
  for (i = 0 ; i < length ; i++){

    index = zoneset[i] ;
    if (fun2j[i] > yield[index]) {
      scale = yield[index] / fun2j[i];
      
      newSxx[i] = newSxx[i] * scale;
      newSyy[i] = newSyy[i] * scale;
      newSzz[i] = newSzz[i] * scale;
      newTxy[i] = newTxy[i] * scale;
      newTxz[i] = newTxz[i] * scale;
      newTyz[i] = newTyz[i] * scale;

    }
  }
}
    

/************************************************************************
 * Function  : StressCheckEpsFail
 * Purpose   : 
 ************************************************************************/
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

#endif 

/************************************************************************
 * Function  : StressZero
 * 
 * Purpose   : 
 ************************************************************************/
void StressZero(real8* newSxx, real8* newSyy, real8* newSzz, 
                real8* newTxy, real8* newTxz, real8* newTyz,
                const real8* fun2j, const real8* shearMod, 
                real8 eosvmax, real8 stresscut,
                const int* zoneset, const real8* vc, int length)
{
  int i, index;

  /* This value 1.e-20 is used to prevent underflow. It is NOT a
     cuttoff. DO NOT TOUCH THIS VALE. */
  real8 stress2 = stresscut*1.e-20 ;
  real8 nstres2 = -stress2 ;
  
  for (i = 0 ; i < length ; i++){
    index = zoneset[i];

    if ((shearMod[index] == 0.0) ||
        (fun2j[i] < stresscut)   ||
        (vc[i] >= eosvmax) ){

      newSxx[i] = 0.0 ;
      newSyy[i] = 0.0 ;
      newSzz[i] = 0.0 ;
      newTxy[i] = 0.0 ;
      newTxz[i] = 0.0 ;
      newTyz[i] = 0.0 ;
    }
#if 1
    if ((newSxx[i]<stress2) && (newSxx[i]>nstres2)) newSxx[i] = 0. ;
    if ((newSyy[i]<stress2) && (newSyy[i]>nstres2)) newSyy[i] = 0. ;
    if ((newSzz[i]<stress2) && (newSzz[i]>nstres2)) newSzz[i] = 0. ;
    if ((newTxy[i]<stress2) && (newTxy[i]>nstres2)) newTxy[i] = 0. ;
    if ((newTxz[i]<stress2) && (newTxz[i]>nstres2)) newTxz[i] = 0. ;
    if ((newTyz[i]<stress2) && (newTyz[i]>nstres2)) newTyz[i] = 0. ;
#endif
  }
}


#if 1
/************************************************************************
 * Function  : StressStrainWork
 * Purpose   : 
 ************************************************************************/
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


/************************************************************************
 * Function  : StressFailureModel
 * Purpose   : 
 ************************************************************************/
void StressFailureModel(real8 *newSxx, real8 *newSyy, real8 *newSzz, 
                        real8 *newTxy, real8 *newTxz, real8 *newTyz,
                        real8 *eps, real8 epsFailModel,
                        const int *zoneset,  int length)
{ 
  if (epsFailModel > 0.0) {
    StressCheckEpsFail(newSxx, newSyy, newSzz, 
                       newTxy, newTxz, newTyz,
                       eps, epsFailModel, 
                       zoneset, length);
  }
}


/************************************************************************
 * Function  : StressCalcShearRatio
 * Purpose   : 
 ************************************************************************/
void StressCalcShearRatio(const real8* shearMod, real8* shearRatio,
                          const int *zoneset, int length)
{
   int i, index ;
   for ( i=0 ; i<length ; i++) {
      index = zoneset[i] ;
      if (shearMod[index] > 0) {
         shearRatio[i] = 1./shearMod[index] ;
      } else {
         shearRatio[i] = 0. ;
      }
   }
}


/************************************************************************
 * Function  : StressStrainHeat
 * Purpose   : 
 ************************************************************************/
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





//************************************************************************
///
/// @note The principal components of the total stress tensor are returned.
/// @param pstress Principal stresses, from largest to smallest
/// @param sx xx-component of deviatoric stress tensor
/// @param sy yy-component of deviatoric stress tensor
/// @param txy xy-component of deviatoric stress tensor
/// @param txz xz-component of deviatoric stress tensor
/// @param tyz yz-component of deviatoric stress tensor
/// @param p Pressure
/// @param acut Magnitude of cutoff for a
/// @pre  none
/// @post none
///
void
StressCalculatePrincipalValues(real8 pstress[3], const real8 sx, const real8 sy,
   const real8 txy, const real8 txz, const real8 tyz, const real8 p,
   const real8 acut)
{
      real8 third = 1.0 / 3.0;

      real8 sz = -(sx + sy) ;

      /* Determine principle values */
      real8 txy2 = txy*txy ;
      real8 txz2 = txz*txz ;
      real8 tyz2 = tyz*tyz ;
      real8 a = (sx*sy + sy*sz + sz*sx - (txy2+txz2+tyz2))/3.0 ;

      /* If a is at all positive here, it will blow up the sqrt
       * below... originally hardwired to -1e-13 */
      a = MIN(a, -acut) ;

      real8 det = sx*sy*sz + 2.0*txy*txz*tyz - sx*tyz2 - sy*txz2 - sz*txy2 ;

      real8 r = sqrt(-pow(a,3.0)) ;

      real8 arg = det/2.0/r ;

      /* Cap the value of arg to (-1 <= arg <= 1) */
      arg = MAX(arg,-1.0) ;
      arg = MIN(arg, 1.0) ;

      real8 theta = acos(arg) ;

      real8 coef = 2.0 * pow(r, third) ;

      /* Calculate eigenvalues of 3x3 matrix */
      real8 p1 = coef*cos(theta/3.0) ;
      real8 p2 = coef*cos((theta+2.0*M_PI)/3.0) ;
      real8 p3 = coef*cos((theta+4.0*M_PI)/3.0) ;

      if ( p2 > p1 ){
         real8 tem = p1 ;
         p1 = p2 ;
         p2 = tem ;
      }
      if ( p3 > p1 ){
         real8 tem = p1 ;
         p1 = p3 ;
         p3 = tem ;
      }
      if ( p3 > p2 ){
         real8 tem = p2 ;
         p2 = p3 ;
         p3 = tem ;
      }

      /* Return principal components of total stress */
      pstress[0] = p1 - p ;
      pstress[1] = p2 - p ;
      pstress[2] = p3 - p ;

}
#endif 
