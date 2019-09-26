
/* Testing the Ansi C++ use in ROSE */

#ifdef __STRICT_ANSI__
// #error "ANSI mode IS defined!"
#else
#error "ANSI mode is NOT defined!"
#endif

#if 0
// Example of non-ansi code:
// Non-ansi code (non compatable function declarations):
   void f(char);
   void f(c) char c; {}
#endif
