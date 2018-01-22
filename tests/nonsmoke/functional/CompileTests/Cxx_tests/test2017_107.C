// This  is allowed for C++ (passes g++ and EDG), but fails for C (gcc and EDG using C98 mode).
void foobar()
   {
     int x;
     switch(x)
        {
          case 0:
             typedef int integer;
        }
   }
