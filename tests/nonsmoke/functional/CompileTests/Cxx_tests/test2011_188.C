
template<class T, class S>
int foo ( T x, S y )
   {
     return 0;
   };

#if 0
// This is a redundant class and is not allowed (but only caught when the function is instantiated).
template<class T, class S>
int foo ( S x, T y )
   {
     return 0;
   };
#endif

void
foobar()
   {
     int i;
     float j;

     foo(i,j);
   }
