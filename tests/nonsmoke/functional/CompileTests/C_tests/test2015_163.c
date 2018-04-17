
int foo(int x[10+10])
   {
     return 0;
   }

int foobar(int x[20])
   {
     return 0;
   }

#if 0
// This is an example of a constant evaluation not handled yet.
// The current support for constnat folding to support improved name mangling is 
// defined to be more narrow than what is possible so that we can avoid having 
// it be too general and beyone the level of implemented support in the constant 
// folding.  This allows for some testing to be done now and expaned as the
// constant folding is improved.
int foobar2(int x[30-10])
   {
     return 0;
   }
#endif
