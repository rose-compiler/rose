void foo()
   {
     int x;
     if (1)
        {
          x = 4;
        }
   }

// Cause a second statement to be in the global scope so we can exercise
// a different branch of the dark token trailing whitespace fixup support.
int abc;

#if 0
void foo(){}
#endif
