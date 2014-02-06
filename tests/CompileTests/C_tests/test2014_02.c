// This test code demonstrates the use of nested function 
// and one defined as "auto" which is not allowed in EDG
// (or at least I have to look into this further).
// Without "auto" EDG still appears to now allow nested 
// functions.  so I need to dig into this further.

static int*** parse_params(char **argv)
   {
  /* This is the only place in busybox where we use nested function.
   * So far more standard alternatives were bigger. */
  /* Auto decl suppresses "func without a prototype" warning: */
  // auto int* alloc_action(int sizeof_struct);
     int* alloc_action(int sizeof_struct);

#if 0
     int* alloc_action(int sizeof_struct)
        {
        }
#endif
   }
