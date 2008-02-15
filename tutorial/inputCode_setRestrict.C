// this code works as input and is output with restrict (no transformation)
int foo(int * __restrict__ a)
{
   return a[0] ;
}

// setRestrict.C transform the input parameter to be a "restrict" pointer
int goo(int *b)
{
  return b[0] ;
}
