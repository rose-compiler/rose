// This test code demonstrates that EDG normalizes the "__decltype()" gnu extension to "decltype()".

template < typename _Size >
void  __fill_n_a ( _Size __n )
   {
     for (__decltype ( __n + 0 ) __niter = __n; __niter > 0; -- __niter )
        {
        }
   }
