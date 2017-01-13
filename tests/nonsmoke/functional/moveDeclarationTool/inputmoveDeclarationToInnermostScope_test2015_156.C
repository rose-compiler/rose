#define MACRO 2

// This example causes this to be all transfromations in the inner most SgBasicBlock.
// So we can't identify representative formatting to use.

void foobar ()
{
   double *a, b;
   int c, d;
   int i, j, i1, i2, e, kkk1;
   double f, g, h, m, a1, a2, kkk2;

   double *array = 0L;

   if ( true ) {

      for ( j = MACRO ; j < 42 ; j++ ) {

         e = j * d ;

         for ( i = MACRO ; i <= 42 ; i++ ) {

            i1 = i + c + e;
            i2 = i1 + d ;

            f = 42.0; // xs[i2] - x[i2] ;
            g = array[i2];
            h =  array[i1];
         }

      }

   } 

}

