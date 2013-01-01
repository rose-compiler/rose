// A gnu C extension found at:
// http://gcc.gnu.org/onlinedocs/gcc-3.2.2/gcc/index.html#Top
// Compiles with EDG, but not with ROSE.

void foo()
   {
     int x = 0;
     switch (x)
        {
          case 0: {}

       // Error in generation of GNU case range (only unparses the first constant in the range: "1 ... 5")
          case 1 ... 5: {}

       // This is an C/C++ error caught in the EDG front-end.
       // case 3: {}

          case 6: {}
          default: {}
        }
   }
