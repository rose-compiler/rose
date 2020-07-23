void foo(double);

void foobar()
   {
#if 0
     foo(0x0'0.0p0);
     foo(0x0'0.0p0F);
     foo(0x0'0.0P+0l);
#endif
   }


