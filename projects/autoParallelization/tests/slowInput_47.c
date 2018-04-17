typedef double real8 ;

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


