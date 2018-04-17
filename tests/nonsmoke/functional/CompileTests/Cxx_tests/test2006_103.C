// Demonstrate of case where the "case 0:" is dropped form the generated code!
int x;
void foo()
   {
// DQ (2/20/2010): This is a error for g++ 4.x compilers (at least g++ 4.2).
#if (__GNUC__ >= 3)
      switch(x) { case 0: default: 0; }
#else
     switch(x) { case 0: default: }
#endif
   }
