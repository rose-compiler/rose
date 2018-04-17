
#define MACRO_B()    \
        dx1 = 42.0;  \
        va += 1.0;

void foobar()
{
   int k, i;
   double dx1;
   double va;

   /* comment comment */

   for ( k = 2; k < 12; k++ ) {

            va = 0.0 ;

            /* comment comment comment C */

            MACRO_B();
  }

}


