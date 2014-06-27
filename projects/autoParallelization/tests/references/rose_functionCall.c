//! Contributed by Jeff Keasler
#include "omp.h" 
typedef double real8;
extern void OtherFunc(int k,real8 *l,real8 *m,real8 *n,real8 *o,real8 *p,real8 q,real8 r,real8 s[3]);

void foo(int istart,int iend,real8 *a,real8 *b,real8 *c,int k,real8 *l,real8 *m,real8 *n,real8 *o,real8 *p)
{
  int k_nom_2;
  int i_nom_1;
  for (i_nom_1 = istart; i_nom_1 <= iend - 1; i_nom_1 += 1) {
    real8 s[3];
    real8 afi = a[i_nom_1];
    real8 bfi = b[i_nom_1];
    OtherFunc(k,l,m,n,o,p,afi,bfi,s);
    
#pragma omp parallel for private (k_nom_2) firstprivate (i_nom_1)
    for (k_nom_2 = 0; k_nom_2 <= 3 - 1; k_nom_2 += 1) {
      c[3 * i_nom_1 + k_nom_2] = s[k_nom_2];
    }
  }
}
