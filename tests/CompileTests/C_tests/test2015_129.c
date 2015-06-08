struct a 
   {
     union b 
        {
          int c;
          int d;
        } e;
     float f;
   } 
#if 1
   g = {.e.c = 3 };
#else
// Normalized form as an alternative.
   g = {.e = { .c = { 3 } } };
#endif
