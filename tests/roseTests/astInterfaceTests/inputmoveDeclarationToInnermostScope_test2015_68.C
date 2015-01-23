
// DQ (1/22/2015): Added ";" to the macro definition.
#define MY_MACRO_2(c) c = 7.0 ; c = 8.0; c = 0.0;

void foobar()
   {
     int i ;
     double *x ;
     double abc ;
     double cc ;

     for ( i = 0 ; i <= 42 ; i++ ) 
        {
          MY_MACRO_2( cc ) ;
        }
   }

