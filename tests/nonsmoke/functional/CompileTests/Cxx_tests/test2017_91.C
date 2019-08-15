// This is legal C++ code (works for GNU, but fails for ROSE (so it is something to debug).
class ABC 
   {
     int xxx;

     public:
     ABC ()
        {
       // xxx = 42;
        }
   };

void foobar() 
   {
     switch (int i = 42)
        {
       // ABC a;
          int x;
          case 1: 
            {
               x = 2;
            // ABC a;
               break;
            }
          int y;
          case 2: 
            // x = 2;
               int z;
               z = x;
          case 3:
          default: ;
        }

     7;
   }
