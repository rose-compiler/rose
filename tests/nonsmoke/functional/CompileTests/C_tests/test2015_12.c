
// int x;

void foo()
   {
  // This works fine, but the case in a conditional fails.
  // unsigned int x = sizeof(struct { float x; });
#if 1
  // This files to unparse the defining declaration (in the conditional).
      if ( sizeof(struct { double x; char y; double z; }) )
        {
        }
#endif
   }

// int y;
