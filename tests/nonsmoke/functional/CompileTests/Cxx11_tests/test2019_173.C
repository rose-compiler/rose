
void foo();

void foobar()
   {
  // BUG: unparsed as: (void )((void )((void )(foo())));
     (const void)(volatile void)(const volatile void)foo();
   }
