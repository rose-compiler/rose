// This test code is testing both only formatting, and
// the correctness of the -rose:merge_decl_assign option.

double* space;

void foobar()
   {
     int def;

     for ( def = 0 ; def < 42; def++ ) 
        {
          int i;
          double  aa, bb, cc, abc ;

          if ( true ) 
             {
               double *a = space;
               double *b = space;

               for ( i = 0 ; i < 42; i++ ) 
                  {
                    aa = a[i];
                    bb = b[i];

                    abc = aa*aa;
                  }
             } 
        }
   }


