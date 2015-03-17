void fallbackSimpleSort ( int* fmap,
                          int* eclass,
                          int   lo,
                          int   hi )
   {
     int j;

     for ( j = 4; j <= hi; j += 4 )
          fmap[j-4] = fmap[j];

   }

