
#define MACRO_IJ(ndx, jp, i, j) \
   j = ndx / jp; \
   i = ndx -j*jp;

#define MACRO_IJK(ndx, jp, kp, i, j, k) \
   k = ndx / kp; \
   j = (ndx - k*kp) / jp; \
   i = ndx -k*kp -j*jp;

void foobar() 
   {
     int offset;

     int len = 0;

     int* zdx = 0;

     for ( int i = 0; i < len; i++ ) 
        {
          int pz_zdx = 7;

          int ii,jj,kk = 0;
          if (3 == 1) 
             {
               MACRO_IJK(pz_zdx, 45, 46, ii, jj, kk);
             }
            else 
             {
               MACRO_IJ(pz_zdx, 42, ii, jj);
             }
        }
   }

