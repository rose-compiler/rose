
#define MACRO_B(aa,k1) \
        double dx1 = aa[k1];  \
        va += 1.0;

void foobar()
{
   int k, i1;
   double *x;
   //   double dx1;
   double va;

   /* comment comment */

   x = 0L;

   for ( k = 2; k < 12; k++ ) {

            va = 0.0 ;

            /* comment comment comment C */

            MACRO_B(x,i1) ;
  }

}


