// Test use of restrict keyword (as used in many programs that don't target gnu and use __restrict__ directly.
struct test_struct
   {
     char c;
     char* restrict p;
   } test_struct_var;
