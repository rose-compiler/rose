struct a 
   {
     struct b 
        {
          int c;
          int d;
        } e;
     float f;
   } 
#if 0
   g = {.e.c = 3 };
#else
// Normalized form as an alternative.
   g = {.e = { .c = { 3 } } };
#endif
