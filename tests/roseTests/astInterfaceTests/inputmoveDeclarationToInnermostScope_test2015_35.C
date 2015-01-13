
#define IJ(ndx, jp, i, j) \
   j = ndx / jp; \
   i = ndx -j*jp;

void foobar() 
   {
     for ( int i = 0; i < 5; i++ ) 
        {
          int pz_zdx = 7;
          int ii,jj,kk = 0;

          if (3 == 1) 
             {
               IJ(pz_zdx, 42, ii, jj);
             }
        }
   }

