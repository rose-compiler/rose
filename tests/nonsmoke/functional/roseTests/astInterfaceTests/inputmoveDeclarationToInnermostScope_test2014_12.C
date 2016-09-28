void fallbackSimpleSort ( int* fmap,
                          int* eclass,
                          int   lo,
                          int   hi )
{
   int i, j, tmp;
   unsigned int ec_tmp;

   if (lo == hi) return;

   if (hi - lo > 3) {
      for ( i = hi-4; i >= lo; i-- ) {
#if 1
         tmp = fmap   [i];
#endif
      }
   }


}
