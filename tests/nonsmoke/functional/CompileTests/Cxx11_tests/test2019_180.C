void foobar()
   {
  // "foo identifies foo as a custom delimiter (in the parenthesis).
  // This works for R (raw) mode, but not for LR mode.
     LR"foo(Hello World)foo";
   }

