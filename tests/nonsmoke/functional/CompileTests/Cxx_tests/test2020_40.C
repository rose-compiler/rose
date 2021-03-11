// DQ (8/20/2020): Attributes are only allowed on the non-defining declaration.
  void x(void)
         __attribute__((overloadable))
         __attribute__((enable_if(true, "")));

// DQ (8/20/2020): Attributes are NOT allowed on the defining declaration.
  void x(void)
      // __attribute__((overloadable))
      // __attribute__((enable_if(true, "")))
         {}
  void f(void) {
    x();
  }
