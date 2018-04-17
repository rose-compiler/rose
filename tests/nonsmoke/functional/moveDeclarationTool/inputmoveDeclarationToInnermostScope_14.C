void fallbackSimpleSort ( int* fmap,
                          int* eclass,
                          int   lo,
                          int   hi )
{
   int i, j, tmp;
   unsigned int ec_tmp;

// for ( j = i+4; j <= hi && ec_tmp > eclass[fmap[j]]; j += 4 )
   for ( j = 4; j <= hi && ec_tmp > eclass[fmap[j]]; j += 4 )
        fmap[j-4] = fmap[j];

} 
