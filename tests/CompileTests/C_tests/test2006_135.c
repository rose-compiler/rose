// Test use of restrict keyword (as used in many programs that don't target gnu and use __restrict__ directly.
struct test_struct
   {
     char c;
#ifdef __GNUC__
  // GNU gcc uses "__restrict__"
     char* __restrict__ p;
#else
  // EDG uses "restrict"
     char* restrict p;
#endif
   } test_struct_var;
