#include <float.h>
#include <math.h>

#define MIN(a, b) ( (a < b) ? a : b)
#define MAX(a, b) ( (a > b) ? a : b)

typedef double real8 ;
   
#if 1
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
