//! Contributed by Jeff Keasler

typedef double real8; 
extern
void OtherFunc(int k, real8 *l, real8 *m,
               real8 *n, real8 *o, real8 *p,
               real8 q, real8 r, real8 s[3]) ;


void foo(int istart, int iend, real8 *a, real8 *b, real8 *c,
         int k, real8 *l, real8 *m, real8 *n, real8 *o,
         real8 *p)
{
   for (int i = istart ; i < iend ; i++) {
      real8 s[3] ;
      real8 afi = a[i] ;
      real8 bfi = b[i] ;
      OtherFunc(k, l, m, n, o, p, afi, bfi, s) ;
      for (int k = 0 ; k < 3 ; k++) {
         c[3*i+k] = s[k] ;
      }
   }
} 
