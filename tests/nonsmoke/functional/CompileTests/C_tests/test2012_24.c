
void foo()
   {
     int argc = 1;

#if 1
  // Test array with non-variable expression
     char * (argarr_original)[argc + 1];
#endif

#if 1
  // Test array with simple variable expression
     char * (argarr_a)[argc];
#endif

#if 1
  // Test multi-dimensional array.
     char * argarr_b[42][argc+1];
#endif

#if 1
     {
    // Hide the VLA declaration in a non function scope.
       char * argarr_c[argc+1];
     }
#endif
   }
