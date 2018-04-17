
void foo()
   {
     const int argc = 1;

#if 0
  /* Test array with non-variable expression (not allowed in Microsoft MSVC) */
     char * (argarr_original)[argc + 1];
#endif

#if 0
  /* Test array with simple variable expression (not allowed in Microsoft MSVC) */
     char * (argarr_a)[argc];
#endif

#if 0
  /* Test multi-dimensional array. (not allowed in Microsoft MSVC) */
     char * argarr_b[42][argc+1];
#endif

#if 0
     {
    /* Hide the VLA declaration in a non function scope. (not allowed in Microsoft MSVC) */
       char * argarr_c[argc+1];
     }
#endif
   }
