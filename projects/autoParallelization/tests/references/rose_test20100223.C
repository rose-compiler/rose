/*
Contributed by Jeff Keasler
Liao 2/23/2010
*/
#include <omp.h> 
typedef double real8;

class ZZ 
{
  public: int len;
  int *values;
}
;

class YY 
{
  public: class YY *next;
  class ZZ *bb;
}
;

class XX 
{
  public: class YY *aa[7];
}
;

void foo(class XX *xx)
{
  int i;
  int b;
  class YY *aa;
  real8 *v1;
  real8 *v2;
  real8 *v3;
  real8 *v4;
  real8 *v5;
  real8 *v6;
  for (b = 0; b <= 8; b += 1) {
    real8 *ddb = 0;
    real8 *dda = 0;
    switch(b){
      case 0:
      aa = xx -> aa[0];
      break; 
      case 1:
      aa = xx -> aa[1];
      break; 
      case 2:
      aa = xx -> aa[2];
      break; 
      case 3:
      aa = xx -> aa[3];
      break; 
      case 4:
      aa = xx -> aa[4];
      break; 
      case 5:
      aa = xx -> aa[5];
      break; 
      case 6:
      aa = xx -> aa[6];
      break; 
      case 7:
      aa = xx -> aa[6];
      break; 
      case 8:
      aa = xx -> aa[6];
      break; 
      default:
/* do nothing */
      ;
    }
    switch(b){
      case 0:;
      case 3:;
      case 6:
      dda = v1;
      ddb = v2;
      break; 
      case 1:;
      case 4:;
      case 7:
      dda = v3;
      ddb = v4;
      break; 
      case 2:;
      case 5:;
      case 8:
      dda = v5;
      ddb = v6;
      break; 
    }
    while(aa){
      
#pragma omp parallel for private (i)
      for (i = 0; i <= aa -> bb -> len - 1; i += 1) {
        int idx = aa -> bb -> values[i];
        ddb[aa -> bb -> values[i]] = dda[aa -> bb -> values[i]];
      }
      aa = aa -> next;
    }
  }
  return ;
}
