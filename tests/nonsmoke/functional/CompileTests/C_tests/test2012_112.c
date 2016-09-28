// This is an interesting code because it passes as a C89 code (default mode in ROSE), yet fails as a C99 code.
// For -rose:C99 mode the "break" is put outside of the "for" loop (which is an error for all versions of C).
// For C89 mode, the code is correctly compiled and the "break" if put into the correct location (in the "for" loop).

// Note that the first fix for this broak test2012_14.c (smaller test code isolated as test2012_114.c).
// This had to do with how we detect an skip over blocks generated from GNU expression statements.

extern int kill (int sig, int x);

void handle_sub(int job, int fg)
   {
     for (;;)
        {
          if (0)
             kill(((__extension__ ({ union { int i; } u; u.i = 42; }) )),77);

          break;
        }
   }

