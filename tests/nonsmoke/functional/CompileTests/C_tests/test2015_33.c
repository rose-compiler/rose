int foo();

unsigned long foobar()
   {
  // This is a bug that is currently normalized to: "return (((foo()?foo() : 1)) - 1);"
     return (foo() ? : 1) - 1;
   }
