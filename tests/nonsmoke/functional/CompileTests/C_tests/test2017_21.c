// This  is allowed for C++ (passes g++ and EDG), but fails for C (gcc and EDG using C98 mode).
// Modified below to pass for C compiler.
void foobar()
   {
     int x;
     switch(x)
        {
       // Passing C code.
          typedef int integer;

          case 0:
            // typedef int integer; // failing C code.
          break;
        }
   }
