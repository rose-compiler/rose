// DQ (1/22/2015): Added ";" to the macro definition.
#define MY_MACRO_1(a,k1) a[k1];

// DQ (1/22/2015): Added ";" to the macro definition.
#define MY_MACRO_2(dx1) dx1 = 7.0 ; dx1 = 8.0 ; dx1 += 1.0 ;


void foobar()
   {
     int i, i1, i2, i3, i4 ;
     double *x, *xs, *ys, *zs ;
     double x4c ;
     double dx1, dy1, dz1 ;
     double dx3, dy3, dz3 ;

     for ( i = 0 ; i <= 42 ; i++ ) 
        {
          x4c = 1.0 * MY_MACRO_1(xs,i1) ;

          MY_MACRO_2( dx3 ) ;

        }
   }

